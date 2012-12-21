<?php
/**
 * @file
 * Make field required. If not present, the feed item will be removed.
 */
$plugin = array(
  'form' => 'brahms_xml_feeds_tamper_parse_citation_form',
  'callback' => 'brahms_xml_feeds_tamper_parse_citation_callback',
  'name' => 'Parse Brahms citation',
  'multi' => 'direct',
  'category' => 'Brahms'
);

function brahms_xml_feeds_tamper_parse_citation_form($importer, $element_key, $settings){
  $form = $sources = array();
  $source_configs = $importer->parser->getMappingSources();
  foreach($importer->processor->config['mappings'] as $mapping){
    $sources[$mapping['source']] = isset($source_configs[$mapping['source']]) ? $source_configs[$mapping['source']]['name'] : $mapping['source'];
  }
  $form['brahmsBibioPageTitle'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['brahmsBibioPageTitle']) ? $settings['brahmsBibioPageTitle'] : key($sources),
    '#options' => $sources,
    '#title' => t('Field to set as the page title'),
    '#description' => t('Select the field you want to contain the tampered/parsed biblio content page title.')
  );
  $form['brahmsBibioSecondaryTitle'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['brahmsBibioSecondaryTitle']) ? $settings['brahmsBibioSecondaryTitle'] : key($sources),
    '#options' => $sources,
    '#title' => t('Field to set as the secondary title'),
    '#description' => t('Select the field you want to contain the tampered/parsed biblio secondary title.')
  );
  $form['brahmsBibioVolume'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['brahmsBibioVolume']) ? $settings['brahmsBibioVolume'] : key($sources),
    '#options' => $sources,
    '#title' => t('Field to set as the biblio volume'),
    '#description' => t('Select the field you want to contain the tampered/parsed biblio volume.')
  );
  $form['brahmsBibioStartPage'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['brahmsBibioStartPage']) ? $settings['brahmsBibioStartPage'] : key($sources),
    '#options' => $sources,
    '#title' => t('Field to set as the start page'),
    '#description' => t('Select the field you want to contain the tampered/parsed biblio start page.')
  );
  return $form;
}

function brahms_xml_feeds_tamper_parse_citation_callback($result, $item_key, $element_key, &$field, $settings){
	$title = brahms_parse_title($field);
	$result->items[$item_key][$settings['brahmsBibioPageTitle']] = $title; // page title
	//$result->items[$item_key]['xpathparser:5'] = $title; // page title
	
	$result->items[$item_key][$settings['brahmsBibioSecondaryTitle']] = $title; //biblio_secondary_title
	//$result->items[$item_key]['xpathparser:16'] = $title; //biblio_secondary_title
	
	$volume = brahms_parse_volume($field);
	$result->items[$item_key][$settings['brahmsBibioVolume']] = $volume; //biblio_volums
	//$result->items[$item_key]['xpathparser:12'] = $volume; //biblio_volums
	
	$start_page = brahms_parse_start_page($field);
	$result->items[$item_key][$settings['brahmsBibioStartPage']] = $start_page; //biblio_section
	//$result->items[$item_key]['xpathparser:17'] = $start_page; //biblio_section
	
	//$author_type = brahms_parse_author_type($field);
	//$result->items[$item_key]['xpathparser:6'] = $authors;
	//dsm($result->items[$item_key]['xpathparser:6']);
}

function brahms_parse_title($field) {
	// find the first number in the string and make title be string up to number
	list($matstr,$matpos) = digpos($field);
	if ($matpos !== false) {
		//  dsm( "The digits [$matstr] were found at position [$matpos] in the string [$field]<br>\n");
		$title = substr($field, 0, $matpos);
	} else {
		// dsm( "No digits were found in the string [$field]<br>\n");
	}
	return $title;
}

function brahms_parse_volume($field) {
	$volume = substr($field, 0, strpos($field, ':'));
	$volume = substr($volume, strrpos($volume, ' '), 100);
	$volume = trim($volume);
	return $volume;
}

function brahms_parse_start_page($field) {
	$start_page = trim(substr($field, strpos($field, ':') +1));
	if (!preg_match('/[A-Za-z]/', $start_page)) {
		if (preg_match('/\./',$start_page)){ 
				$start_page = substr($start_page, 0, strpos($start_page, '.'));
			} else {
				$start_page = $start_page;
			}	
		return $start_page;
	} 
}

function brahms_parse_author_type($field) {
	$auth_type = "14"; //editor
}

/*
* http://forums.phpfreaks.com/topic/220097-how-to-find-first-number-0-9-in-a-string/
*/
function digpos($str) {
  $pat = '(\d+)';
  preg_match($pat,$str,$matches);
  return(array($matches[0],strpos($str,$matches[0])));
}