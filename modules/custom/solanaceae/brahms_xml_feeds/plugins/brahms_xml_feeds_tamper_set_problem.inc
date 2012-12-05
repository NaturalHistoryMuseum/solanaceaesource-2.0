<?php
/**
 * feeds_tamper API definition for drupal role mapping
 * Allow for mapping values from an allowed itis ranking terms to Brahms terms
 */
$plugin = array(
  'form' => 'brahms_xml_feeds_tamper_set_problem_form',
  'callback' => 'brahms_xml_feeds_tamper_set_problem_callback',
  'validate' => 'brahms_xml_feeds_tamper_set_problem_validate',
  'name' => 'Count empty and set problem',
  'multi' => 'direct',
  'category' => 'Filter',
  'description' => 'This will allow you to debug a xml node'
);

/**
 * Form for building the Brahms to Scratchpad rank term mapping
 */
function brahms_xml_feeds_tamper_set_problem_form($importer, $element_key, $settings){
  $form = $sources = array();
  $source_configs = $importer->parser->getMappingSources();
  foreach($importer->processor->config['mappings'] as $mapping){
    $sources[$mapping['source']] = isset($source_configs[$mapping['source']]) ? $source_configs[$mapping['source']]['name'] : $mapping['source'];
  }
  $form['source'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['source']) ? $settings['source'] : key($sources),
    '#options' => $sources,
    '#title' => t('Value to count')
  );
  return $form;
}

function brahms_xml_feeds_tamper_set_problem_validate(&$settings){}

/**
 * Function to process the value conversion
 * Called by the feed (via Feed Tamper) to modify data.
 */
function brahms_xml_feeds_tamper_set_problem_callback($result, $item_key, $element_key, &$field, $settings){
  $countSource = $result->items[$item_key][$settings['source']];
  // count of rank field was zero so = problem
  if($countSource == 0){
    $field = "Problem " . $field;
    $result->items[$item_key]['xpathparser:0'] = "Problem " . $result->items[$item_key]['xpathparser:0'];
    $result->items[$item_key]['xpathparser:5'] = "Problem " . $result->items[$item_key]['xpathparser:5'];
    $result->items[$item_key]['xpathparser:22'] = "Problem ";
  }else{
    $field = $field;
  }
}
?>