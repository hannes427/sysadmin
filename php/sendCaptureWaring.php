#!/usr/bin/env php
<?php
/**
 *
 * This script is run by fwconsole job. It sends warining by email if a packetcapture runs for more than 1 day
 *
 */
include '/etc/freepbx.conf';
global $db;
$sql = "SELECT COUNT(id) FROM systemadmin_packetcapture WHERE date<subdate(CURRENT_TIMESTAMP, 1) AND stopped = 'no'";
$stmt = $db->prepare($sql);
$stmt->execute();
$result = $stmt->fetch(\PDO::FETCH_ASSOC);
if ($result['COUNT(id)'] > 0) {
    $from = 'freepbx@freepbx.local';
    $count = $result['COUNT(id)'];
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
            $email->subject("Packetcapture Warning on host $system_id");
            $content = "Hi,<br><br>there are $count packetcapture jobs running for longer than 24 hours on host $system_id. If you are sure that this is intended you can ignore this message.";
            $email->message($content);
            $email->set_priority(1);
            $email->send();
        }
    }
}
?>
