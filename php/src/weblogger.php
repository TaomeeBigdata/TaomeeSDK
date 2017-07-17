<?php
//提供http版本的logger 只支持人数人次计算
//请求方式 http://misc.taomee.com/weblogger.php?gameid=$gid&stid=$stid&sstid=$sstid&uid=$uid
require_once("statlogger.php");

$gameid = -1;
if(isset($_REQUEST['gameid']))
{
    $gameid = $_REQUEST['gameid'];
}

$stid="unknown";
if(isset($_REQUEST['stid']))
{
    $stid = urldecode($_REQUEST['stid']);
    if(!is_utf8($stid))
        $stid = "stidnotutf8";
}

$sstid="unknown";
if(isset($_REQUEST['sstid']))
{
    $sstid = urldecode($_REQUEST['sstid']);
    if(!is_utf8($sstid))
        $sstid = "sstidnotutf8";
}

$uid=-1;
if(isset($_REQUEST['uid']))
{
    $uid = $_REQUEST['uid'];
}

stat_logger($gameid, -1, -1, -1, $uid, -1, $stid, $sstid);


?>
