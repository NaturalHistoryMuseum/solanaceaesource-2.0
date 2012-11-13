<?php

/**
 * @file
 * FeedsFetcher plugin for BRAHMS datasets.
 */
/**
 * BrahmsFetcherResult result of BrahmsFetcher.
 */
class BrahmsFetcherResult extends FeedsFetcherResult {

  protected $database_identifier;

  /**
   * Constructor.
   */
  public function __construct($database_identifier = NULL) {
    $this->database_identifier = $database_identifier;
    parent::__construct('');
  }

  /**
   * Overrides FeedsFetcherResult::getRaw();
   * This is where we get data directly from BRAHMS and assign it to $this->raw as a JSON string.
   */
  public function getRaw() {
    $brahms = new Brahms($this->database_identifier);
    $brahms->setData();
    return $this->sanitizeRaw(json_encode($brahms->data));
  }
}
/**
 * BrahmsFetcher fetches data from BRAHMS.
 */
class BrahmsFetcher extends FeedsFetcher {

  /**
   * Implements FeedsFetcher::fetch().
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    return new BrahmsFetcherResult($source_config['source']);
  }
  /**
   * Overrides FeedsPlugin::sourceForm()
   */
  public function sourceForm($source_config) {
    global $databases;
    $options = array();
    foreach ($databases as $id => $db) {
      if (isset($db['default']['type']) && $db['default']['type'] == 'brahms') {
        $options[$id] = $db['default']['title'];
      }
    }
    $form = array();
    $form['source'] = array(
      '#type' => 'select',
      '#title' => t('Database identifier'),
      '#description' => t('Choose the BRAHMS database you would like to fetch data from.'),
      '#default_value' => isset($source_config['source']) ? $source_config['source'] : '',
      '#options' => $options,
      '#required' => TRUE,
    );
    return $form;
  }

}