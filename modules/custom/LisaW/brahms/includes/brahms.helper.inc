<?php

/**
 * @file
 * Includes helper function for that are shared between
 * BRAHMS install and module files.
 */
/**
 * Helper function to define bundles.
 *
 */
function _brahms_installed_bundles() {
  $bundles = array(
    'taxonomy_term' => array(
      'brahms_taxon_name' => array(
        'name' => t('BRAHMS Taxon Name'),
        'machine_name' => 'brahms_taxon_name',
        'description' => t("A scientific plant name that is governed by or appears "
                         . "to be governed by the International Code of Botanical "
                         . "Nomenclature."),
      ),
    ),
  );
  return $bundles;
}