#!/usr/bin/env php
<?php
/**
 *
 * fwconsole job executes this file. In case a no longer running packetcapture was not stopped using the web ui (e.g. system reboot),  this file updates the colum 'stopped' in the table systemadmin_packetcapture
 *
 */
include '/etc/freepbx.conf';
global $db;
$sql = "SELECT date, id FROM systemadmin_packetcapture WHERE stopped = 'no'";
$results = $db->getAll($sql, DB_FETCHMODE_ASSOC);
foreach($results AS $result) {
    $pid = FreePBX::Systemadmin()->getCapturePid($result['id']);
    if ($pid == -1) {
        $update = "UPDATE systemadmin_packetcapture SET stopped='yes' WHERE id = '$result[id]'";
        $stmt = $db->prepare($update);
        $stmt->execute();
    }
}
?>

