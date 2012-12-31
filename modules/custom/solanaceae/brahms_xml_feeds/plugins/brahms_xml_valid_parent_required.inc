<?php
/**
 * @file
 * Make field required. If not present, the feed item will be removed.
 */
$plugin = array(
  'form' => 'brahms_xml_valid_parent_required_form',
  'callback' => 'brahms_xml_valid_parent_required_callback',
  'name' => 'Required parent field',
  'multi' => 'direct',
  'category' => 'Filter'
);

function brahms_xml_valid_parent_required_form($importer, $element_key, $settings){
  $form = $sources = array();
  $source_configs = $importer->parser->getMappingSources();
  foreach($importer->processor->config['mappings'] as $mapping){
    $sources[$mapping['source']] = isset($source_configs[$mapping['source']]) ? $source_configs[$mapping['source']]['name'] : $mapping['source'];
  }
  $form['countSource'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['countSource']) ? $settings['countSource'] : key($sources),
    '#options' => $sources,
    '#title' => t('Node to check if count is zero'),
    '#description' => t('If this count is zero set the following items')
  );
  $form['defaultProblemGuid'] = array(
    '#type' => 'textfield',
    '#title' => t('Default GUID for the problem guid'),
    '#description' => t('What is the GUID of the problem term which you want these grouped under'),
    '#default_value' => isset($settings['defaultProblemGuid']) ? $settings['defaultProblemGuid'] : 'GENUSCODE-PBISOLANUM-9'
  );
  $form['parentGuid'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['parentGuid']) ? $settings['parentGuid'] : key($sources),
    '#options' => $sources,
    '#title' => t('The parent guid'),
    '#description' => t('Set this node to be the default GUID above')
  );
  $form['descField'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['descField']) ? $settings['descField'] : key($sources),
    '#options' => $sources,
    '#title' => t('The term description field'),
    '#description' => t('Set this node to be the term description field')
  );
  $form['parentName'] = array(
    '#type' => 'radios',
    '#default_value' => isset($settings['parentName']) ? $settings['parentName'] : key($sources),
    '#options' => $sources,
    '#title' => t('The parent name field'),
    '#description' => t('Set this node to be the expected parent name')
  );
  return $form;
}

function brahms_xml_valid_parent_required_callback($result, $item_key, $element_key, &$field, $settings){
  $countSource = $result->items[$item_key][$settings['countSource']];
  // count of rank field was zero so = problem
  if($countSource == 0){
    // unset($result->items[$item_key]);
    // $result->items[$item_key]['xpathparser:21'] = $settings['defaultProblemGuid'];
    $result->items[$item_key][$settings['parentGuid']] = $settings['defaultProblemGuid'];
    // $result->items[$item_key]['xpathparser:11'] =
    // $result->items[$item_key]['xpathparser:11'] . ' Problem with parent: ' .
    // $result->items[$item_key]['xpathparser:19'];
    $result->items[$item_key][$settings['descField']] = $result->items[$item_key][$settings['descField']] . '   Problem with parent: ' . $result->items[$item_key][$settings['parentName']];
  }
}