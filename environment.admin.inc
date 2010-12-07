<?php
// $Id$

/**
 * @file
 * Settings for Environment module
 */

function environment_admin_settings() {
  $form = array();

  $env_override = variable_get('environment_override', NULL);
  if ($env_override) {
    $form['environment'] = array(
      '#type'  => 'item',
      '#title' => t('Current Environment'),
      '#value' => check_plain(variable_get('environment', 'none')),
    );
    $form['environment_override'] = array(
      '#type'  => 'item',
      '#title' => t('Environment Override'),
      '#value' => t('Active'),
    );
  }
  else {
    $form['environment'] = array(
      '#type'          => 'select',
      '#title'         => t('Environment'),
      '#options'       => drupal_map_assoc(array_keys(environment_get())),
      '#default_value' => variable_get('environment', NULL),
    );
  }
  $form['environment_require_override'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Require environment override'),
    '#description'   => t('Used to require that an environment is set in the settings.php file.'),
    '#default_value' => variable_get('environment_require_override', FALSE),
  );

  $form['#submit'][] = 'environment_switch_environment_submit';
  return system_settings_form($form);
}

/**
 * Submit callback to switch environment if changed.
 */
function environment_switch_environment_submit($form, &$form_state) {
  if (!$form_state['values']['environment_require_override']
    && $form['environment']['#default_value'] != $form_state['values']['environment']) {

    $result = environment_switch($values['environment']);
    foreach ($result['messages'] as $message) {
      drupal_set_message(t(check_plain($message)));
    }
  }
}

/**
 * Environment switch form callback.
 *
 * @param $environment
 *  The environment being switched to.
 */
function environment_switch_confirm(&$form_state, $environment) {
  if (!environment_get($environment)) {
    drupal_set_message(t('Invalid environment "%environment". You cannot switch to an undefined environment.', array('%environment' => $environment)), 'warning');
    drupal_goto('admin/settings/environment');
  }
  return confirm_form(array('environment' => array('#type' => 'hidden', '#value' => $environment)), t('Are you sure you want to switch the current environment?'), 'admin/settings/environment',
    t('This action switches the current environment to "%env". This kind of change is as risky as updating your site. This action cannot be undone.', array('%env' => $environment)),
    t('Switch environment'), t('Cancel')
  );
}

/**
 * Handler for switch environment confirmation.
 */
function environment_switch_confirm_submit($form, &$form_state) {
  environment_switch($form['environment']['#value']);
  $form_state['redirect'] = 'admin/settings/environment';
}