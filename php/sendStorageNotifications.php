#!/usr/bin/env php
<?php
/**
 *
 * fwconsole job executes this file. This file sends notfification when used space on defined disks reaches defined threshold
 *
 */
include '/etc/freepbx.conf';
global $db;
$disks = array();
$storagesettings = array();
$hour = date("H");
$day_of_week = date("w");
$day = date("d");
if (function_exists('systemadminFetchStorageConfig')) {
    $storagesettings = systemadminFetchStorageConfig();
    $sql_get_disks = 'SELECT value FROM systemadmin_settings WHERE `key` = \'monitor_devices\'';
    $stmt_disks = $db->prepare($sql_get_disks);
    $stmt_disks->execute();
    if ($result_disks = $stmt_disks->fetch(\PDO::FETCH_ASSOC)) {
        $disks = json_decode($result_disks['value'], true);
    }
    $threshold_full_disks = array();
    foreach($disks AS $disk) {
        $output_disks = "";
        $output_disks_rc = "";
        exec("/usr/bin/df | grep $disk$ | awk -F' ' {'print $5'}", $output_disks, $output_disks_rc);
        if ($output_disks_rc == 0) {
            if ($output_disks[0] >= $storagesettings['threshold_2']) {
                $threshold_full_disks[] = array('disk' => $disk, 'full' => $output_disks[0]);
            }
            else if ($output_disks[0] >= $storagesettings['threshold_1']) {
                $threshold_full_disks[] = array('disk' => $disk, 'full' => $output_disks[0]);
            }
        }
    }
    if ((($storagesettings['email_frequency_2'] == "hourly" || $storagesettings['email_frequency_1'] == "hourly") && count($threshold_full_disks) > "0") || (($storagesettings['email_frequency_2'] == "daily" || $storagesettings['email_frequency_1'] == "daily") && ($hour == "07" && count($threshold_full_disks) > "0")) || (($storagesettings['email_frequency_2'] == "weekly" || $storagesettings['email_frequency_1'] == "weekly") && ($hour == "07" && $day_of_week == "1" && count($threshold_full_disks) > "0")) || (($storagesettings['email_frequency_2'] == "monthly" || $storagesettings['email_frequency_1'] == "monthly") && ($hour == "07" && $day == "01" && count($threshold_full_disks) > "0"))) {
        if (function_exists('systemadminFetchEmail')) {
            $emails = systemadminFetchEmail();
            //Check that what we got back above is a email address
            if (!empty($emails['fromemail']) && filter_var($emails['fromemail'],FILTER_VALIDATE_EMAIL)) {
                $from = $emails['fromemail'];
            }
            if (!empty($emails['storageemail']) && filter_var($emails['storageemail'],FILTER_VALIDATE_EMAIL)) {
                $system_id = \FreePBX::Config()->get("FREEPBX_SYSTEM_IDENT");
                $recipient =$emails['storageemail'];
                $from = filter_var(FreePBX::Config()->get('AMPBACKUPEMAILFROM'),FILTER_VALIDATE_EMAIL)?FreePBX::Config()->get('AMPBACKUPEMAILFROM'):$from;
                $email = new \CI_Email();
                $email->set_mailtype("html");
                $email->from($from,$system_id);
                $email->to($recipient);
                $email->subject("Storage Warning on host $system_id");
                $content = "Hi,<br><br>The following disks exceeed the predefined storage threshold:<br>\n";
                foreach($threshold_full_disks AS $disk) {
                    $content .= "$disk[disk] ($disk[full])<br>\n";

                }
                $email->message($content);
                $email->set_priority(1);
                $email->send();
            }
        }
    }
}
?>
