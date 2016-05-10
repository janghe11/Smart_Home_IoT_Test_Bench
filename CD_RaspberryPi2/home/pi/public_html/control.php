<?php
    /* ====================$andCommGet variable parameters====================
    * 'u' : Doorlock unlock 
    * 'l' : Doorlock lock
    * 't' : Boiler temp increase
    * 'b' : Boiler temp decrease
    * 'g' : Loosen gas valve
    * 'v' : Fasten gas valve
    */
    $andClientId = null;        // Check client_id(Android Saved Id, Default : null)
    $andCommGet = null;         // Get command from Android for AVR (Default : null)
    $andModeGet = null;            // 1 -> Status check mode, 2 -> AVR control mode (Default : null)

    $rpiClientId = "capstonemp12d160422r002"; // Check client_id(RaspberryPi Saved Id)
    $rpiId = "rpi2d160423w10";
    $avrId = "avr8535d160423w04";

    $rpiCmdRightChecker = 0;    // Check command is received from android. (Default : 0, Success : 1)
    $rpiCmdFetchChecker = 0;    // Check avr_daemon inserted data to mariadb. (Default : 0, Success : 1)
    
    $mariaHost = "localhost";
    $mariaUser = "root";
    $mariaPassword = "raspberry";
    $mariaDBName = "and_rpi_avr";
    $mariaPreData = null;

    $avrBoilerTemp = null;
    $avrDevId = "/dev/ttyUSB0"; // Device location of AVR USB

    function rpiResetData($andClientId, $andModeGet, $andCommGet, $avrBoilerTemp)
    {
        $andClientId = null;
        $andModeGet = null;
        $andCommGet = null;
        $avrBoilerTemp = null;
    }

    function rpiAndCommCheck($andCommGet, $avrBoilerTemp)
    {
        switch($andCommGet) {
        case 'u':
            break;
        case 'l':
            break;
        case 't':
            if(($avrBoilerTemp < 18) || ($avrBoilerTemp > 30)) {
                echo("Couldn't receive boiler temperature from android.\n");
                $andCommGet = null;
                $avrBoilerTemp = null;
                $errorCheck = 1;
            }
            break;
        case 'b':
            if(($avrBoilerTemp < 18) || ($avrBoilerTemp > 30)) {
                echo("Couldn't receive boiler temperature from android.\n");
                $andCommGet = null;   
                $avrBoilerTemp = null;
                $errorCheck = 1;
            }
            break;
        case 'g':
            break;
        case 'v':
            break;
        default:
            echo("Android commnad token to AVR error.\n");
            $andClientId = null;
            $andCommGet = null;
            return 0;
        }
        echo ("rpiAndCommCheck confirmed.\n");
        return 1;
    }

    function andGetMaria($mariaHost, $mariaUser, $mariaPassword, $mariaDBName)
    {
        $mariaToJson = array();
        $mariaConnect = mysql_connect($mariaHost, $mariaUser, $mariaPassword) or die("MariaDB Connect Error. Please check database name.(From andGetMaria)\n");
        $mariaDBConnect = mysql_select_db($mariaDBName, $mariaConnect);
        mysql_query("set names utf8");
        $mariaQuery = "select * from rpi_avr Order By cid DESC";
        $getMariaData = mysql_query($mariaQuery, $mariaConnect);
            
        while($mariaRow = mysql_fetch_array($getMariaData, MYSQL_ASSOC)) {
            $mariaRowArray['cid'] = $mariaRow['cid'];
            $mariaRowArray['client_id'] = $mariaRow['client_id'];
            $mariaRowArray['rpi_id'] = $mariaRow['rpi_id'];
            $mariaRowArray['avr_id'] = $mariaRow['avr_id'];
            $mariaRowArray['avr_data'] = $mariaRow['avr_data'];
            $mariaRowArray['avr_param'] = $mariaRow['avr_param'];
            $mariaRowArray['rpi_time'] = $mariaRow['rpi_time'];
                   
            array_push($mariaToJson, $mariaRowArray);
        }
            
        echo json_encode($mariaToJson);
        mysql_free_result($getMariaData);
        mysql_close($mariaConnect);
    }

    function rpiGetMariaPre($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $mariaPreData)
    {
        // Check status before sending command to AVR
        $mariaConnect = mysql_connect($mariaHost, $mariaUser, $mariaPassword) or die("MariaDB Connect Error. Please check database name.(From avrSetComm)\n");
        $mariaDBConnect = mysql_select_db($mariaDBName, $mariaConnect);
        mysql_query("set names utf8");
        $mariaQuery = "select * from rpi_avr Order By cid DESC";
        $getMariaData = mysql_query($mariaQuery, $mariaConnect);
        
        $mariaRow = mysql_fetch_array($getMariaData, MYSQL_ASSOC);
        $mariaPreData = $mariaRow['cid'];
        echo "mariaPreData['cid'] :";
        echo $mariaPreData;
        echo "\n";
        mysql_close($mariaConnect);
    }

    function rpiCheckAvrFetch($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $andCommGet, $mariaPreData)
    {   
        $mariaFetchData = array();
        $mariaConnect = mysql_connect($mariaHost, $mariaUser, $mariaPassword) or die("MariaDB Connect Error. Please check database name.(From rpiCheckAvrFetch)\n");
        $mariaDBConnect = mysql_select_db($mariaDBName, $mariaConnect);
        mysql_query("set names utf8");
        $mariaQuery = "select * from rpi_avr Order By cid DESC";
        $getMariaData = mysql_query($mariaQuery, $mariaConnect);
        
        $mariaRow = mysql_fetch_array($getMariaData, MYSQL_ASSOC);
        $mariaFetchData['cid'] = $mariaRow['cid'];
        $mariaFetchData['avr_data'] = $mariaRow['avr_data'];
        echo "mariaFetchData['cid'] :";
        echo $mariaFetchData['cid'];
        echo "\n";
        echo "mariaFetchData['avr_data'] :";
        echo $mariaFetchData['avr_data'];
        echo "\n";
        
        mysql_close($mariaConnect);
        
        if(in_array($mariaPreData, $mariaFetchData)) {
                return 0;
        } else {
            if(in_array($andCommGet, $mariaFetchData)) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    function avrSetComm($andCommGet, $avrDevId, $avrBoilerTemp)
    {
        // Send command to AVR
        $avrSendComm = "echo $andCommGet >> $avrDevId";        // Make command to AVR
        exec($avrSendComm);                                    // Send data to AVR
        
        if($avrBoilerTemp != null)
            $avrSendComm = "echo $avrBoilerTemp >> $avrDevId"; // Make boiler temperature command to AVR
        exec($avrSendComm);    
    }

    //========== Start Main ==========//
    $andClientId = $_POST["andClientId"];
    $andModeGet = $_POST["andModeSet"];
    $andCommGet = $_POST["avrCommSet"];
    $avrBoilerTemp = $_POST["avrBoilerSet"];

    // Check client_id between saved raspberry data and received android data
    $cliendIDChecker = strcmp($andClientId, $rpiClientId);
    if($cliendIDChecker != 0) {
        rpiResetData($andClientId, $andModeGet, $andCommGet, $avrBoilerTemp);
        echo("Client id sent from android not matched! andClientId : $andClientId, rpiClientId : $rpiClientId\n");
    }

    if($andModeGet == 1) {                      // Mode 1 -> Status check mode.
        andGetMaria($mariaHost, $mariaUser, $mariaPassword, $mariaDBName);
    } else if($andModeGet == 2) {               // Mode 2 -> Android to AVR set command.
        $rpiCmdRightChecker = rpiAndCommCheck($andCommGet, $avrBoilerTemp);
        
        if($rpiCmdRightChecker == true) {
            // Save previous cid from database
            rpiGetMariaPre($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $mariaPreData);
            // Send command to AVR
            avrSetComm($andCommGet, $avrDevId, $avrBoilerTemp);
            
            // loop until command fetch checker true
            while($rpiCmdFetchChecker == false) {
                echo("First rpiComFetchChekcer : $rpiCmdFetchChekcer\n");
                $rpiCmdFetchChecker = rpiCheckAvrFetch($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $andCommGet, $mariaPreData);
                echo("AVR data not fetched yet. rpiCmdFetchChecker : $rpiCmdFetchChecker\n");
                sleep(1);
            }
            
            echo("AVR data fetched!. rpiCmdFetchChecker : $rpiCmdFetchChecker\n");
            andGetMaria($mariaHost, $mariaUser, $mariaPassword, $mariaDBName);
        } else {                                // andCommGet failed.
            rpiResetData($andClientId, $andModeGet, $andCommGet, $avrBoilerTemp);
            echo("Occured error while receiving command data from android. andCommGet : $andCommGet \n");
        }
    } else {                                    // andModeGet failed.
        rpiResetData($andClientId, $andModeGet, $andCommGet, $avrBoilerTemp);
        echo("Android mode receive data error!\n");
    }

    rpiResetData($andClientId, $andModeGet, $andCommGet, $avrBoilerTemp);
?>