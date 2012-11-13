<?php

/**
 * @file
 * Get data from BRAHMS.
 */
// dwc:nomenclaturalCode see http://rs.tdwg.org/dwc/terms/index.htm#Taxon
define('BRAHMS_NOMENCLATURAL_CODE', 'ICBN');
// dwc:basisOfRecord
define('BRAHMS_BASIS_TAXON', 'Taxon');

/**
 * Base class for querying BRAHMS databases.
 */
class Brahms {

  protected $database_identifier;
  protected $database;
  public $data;
  public $errors;
  public $ping;

  public function __construct($database_identifier) {
    $this->database_identifier = $database_identifier;
    $this->setDatabase();
  }

  public function setDatabase() {
    global $databases;
    $this->database = $databases[$this->database_identifier];
  }

  public function setData() {
    //$this->setPeople();
    $this->setTaxa();
  }

  public function setDatum($datum, $basis_of_record) {
    $datum['dwc:datasetName'] = $this->database['default']['title'];
    $datum['dwc:datasetID'] = $this->database['default']['guid'];
    $datum['dcterms:type'] = '';
    $datum['dcterms:rights'] = '';
    $datum['dcterms:rightsHolder'] = '';
    $datum['dcterms:language'] = '';
    $datum['dcterms:bibliographicCitation'] = '';

    $this->data[$basis_of_record][] = $datum;
  }

  /**
   * Extract people from BRAHMS.
   */
  public function setPeople() {
    $result = $this->query('SELECT * FROM people');
    foreach ($result as $row) {
      $person['localIdentifier'] = $row->id;
    }
  }

  /**
   * Extract taxa from BRAHMS.
   */
  public function setTaxa() {
    // We are getting species or infraspecies and their associated genera and families data.
    $result = $this->query('SELECT f.facode,
                                f.faname,
                                pvf.namestring AS fauthority,
                                f.citation AS fcitation,
                                g.gecode,
                                g.gename,
                                g.citation AS gcitation,
                                g.nomnote AS gnomnote,
                                pvg.namestring AS gauthority,
                                s.spnumber,
                                s.sp1,
                                pvs1.id AS sp1authorityid,
                                pvs1.namestring AS sp1authority,
                                s.rank1,
                                s.sp2,
                                pvs2.id AS sp2authorityid,
                                pvs2.namestring AS sp2authority,
                                s.rank2,
                                s.sp3,
                                pvs3.id AS sp3authorityid,
                                pvs3.namestring AS sp3authority,
                                s.fullname AS sfullname,
                                s.citation AS scitation
                            FROM species AS s
                            LEFT JOIN peopleview AS pvs1 ON s.aucode1 = pvs1.id
                            LEFT JOIN peopleview AS pvs2 ON s.aucode2 = pvs2.id
                            LEFT JOIN peopleview AS pvs3 ON s.aucode3 = pvs3.id
                            JOIN genus AS g ON s.gecode = g.gecode
                            LEFT JOIN peopleview AS pvg ON g.aucode = pvg.id
                            JOIN family AS f ON g.facode = f.facode
                            LEFT JOIN peopleview AS pvf ON f.aucode = pvf.id
                            WHERE spnumber > 0');
    if (!$result) return FALSE;
    $genera = array();
    $families = array();
    foreach ($result as $taxon) {

      // SQL trim doesn't always work, likely due to special characters, so trim here.
      $species['familyID'] = $taxon->facode;
      $species['genusID'] = $taxon->gecode;

      $species['speciesID'] = $taxon->spnumber;
      $species['specificEpithetAuthorshipID'] = $taxon->sp1authorityid;
      $species['specificEpithetAuthorship'] = trim($taxon->sp1authority);

      $species['firstInfraspecificEpithetRank'] = trim($taxon->rank1);
      $species['firstInfraspecificEpithet'] = trim($taxon->sp2);
      $species['firstInfraspecificEpithetAuthorshipID'] = trim($taxon->sp2authorityid);
      $species['firstInfraspecificEpithetAuthorship'] = trim($taxon->sp2authority);

      $species['secondInfraspecificEpithetRank'] = trim($taxon->rank2);
      $species['secondInfraspecificEpithet'] = trim($taxon->sp3);
      $species['secondInfraspecificEpithetAuthorshipID'] = trim($taxon->sp3authorityid);
      $species['secondInfraspecificEpithetAuthorship'] = trim($taxon->sp3authority);

      $species['dwc:family'] = trim($taxon->faname);
      $species['dwc:genus'] = trim($taxon->gename);
      $species['dwc:specificEpithet'] = trim($taxon->sp1);
      $species['dwc:higherClassification'] = "{$species['dwc:family']};{$species['dwc:genus']}";
      $species['dwc:namePublishedIn'] = trim($taxon->scitation);

      if (! empty($species['secondInfraspecificEpithet'])) {
        $species['dwc:infraspecificEpithet'] = $species['secondInfraspecificEpithet'];
        $species['dwc:verbatimTaxonRank'] = $species['secondInfraspecificEpithetRank'];
        $species['dwc:higherClassification'] .= ";{$species['dwc:specificEpithet']};{$species['firstInfraspecificEpithet']}";
      }
      elseif (!empty($species['firstInfraspecificEpithet'])) {
        $species['dwc:infraspecificEpithet'] = $species['firstInfraspecificEpithet'];
        $species['dwc:verbatimTaxonRank'] = $species['firstInfraspecificEpithetRank'];
        $species['dwc:higherClassification'] .= ";{$species['dwc:specificEpithet']}";
      }
      else {
        $species['dwc:verbatimTaxonRank'] = 'species';
      }

      // Add genera.
      if (! in_array($taxon->gecode, $genera) && ! empty($species['dwc:genus']) && $taxon->gecode > 0) {
        $genus['familyID'] = $species['familyID'];
        $genus['genusID'] = $species['genusID'];

        $genus['dwc:verbatimTaxonRank'] = 'genus';
        $genus['dwc:higherClassification'] = $species['dwc:family'];
        $genus['dwc:family'] = $species['dwc:family'];
        $genus['dwc:genus'] = $species['dwc:genus'];
        $genus['dwc:namePublishedIn'] = trim($taxon->gcitation);
        $this->setTaxon($genus);
        unset($genus);
        $genera[] = $species['genusID'];
      }

      // Add families.
      if (! in_array($taxon->facode, $families) && !empty($species['dwc:family']) && $taxon->facode > 0 ) {
        $family['familyID'] = $species['familyID'];

        $family['dwc:verbatimTaxonRank'] = 'family';
        $family['dwc:family'] = $species['dwc:family'];
        $family['dwc:namePublishedIn'] = trim($taxon->fcitation);
        $this->setTaxon($family);
        unset($family);
        $families[] = $species['familyID'];
      }

      $this->setTaxon($species);
      unset($species);
    }
  }

  /**
   * Add individual taxon to this object taxa property.
   * @param $taxon
   *     Array of properties and values for an individual taxon.
   * @param $code
   *     String abbreviation of nomenclatural code.
   */
  private function setTaxon($taxon, $code = BRAHMS_NOMENCLATURAL_CODE) {

    $taxon['dwc:basisOfRecord'] = BRAHMS_BASIS_TAXON;
    $taxon['dwc:nomenclaturalCode'] = $code;
    $this->setDatum($taxon, BRAHMS_BASIS_TAXON);
  }

  /**
   * Query BRAHMS.
   * @param $sql
   *     String select sql statement for OPENQUERY.
   */
  private function query($sql) {
    $was_active = db_set_active($this->database_identifier);
    try {
      $result = db_query("SELECT * FROM OPENQUERY({$this->database['default']['database']}, '{$sql}')");
    }
    catch (Exception $e) {
      watchdog('brahms', 'There was a problem querying BRAHMS database %database: !exception',
        array('%database' => $this->database_identifier, '!exception' => $e->getMessage()), WATCHDOG_ALERT);
      $result = FALSE;
      $this->errors[] = $e->getMessage();
    }
    db_set_active($was_active);
    return $result;
  }

  /**
   * Ping BRAHMS to check it is available.
   */
  public function ping() {
    $result = $this->query('SELECT count(spnumber) FROM species');
    if ($result) $this->ping = $result->fetchField();
  }

}