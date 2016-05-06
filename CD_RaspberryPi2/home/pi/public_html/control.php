<?php
    /* ====================$andCommGet variable parameters====================
    * 'u' : Doorlock unlock 
    * 'l' : Doorlock lock
    * 't' : Boiler temp increase
    * 'b' : Boiler temp decrease
    * 'g' : Loosen gas valve
    * 'v' : Fasten gas valve
    */
    $andClientId = "capstonemp12d160422r002";        // Check client_id(Android Saved Id, Default : null)
    $andCommGet = 'g';         // Get command from Android for AVR (Default : null)
    $andModeGet = 2;            // 1 -> Status check mode, 2 -> AVR control mode (Default : null)

    $rpiClientId = "capstonemp12d160422r002"; // Check client_id(RaspberryPi Saved Id)
    $rpiCmdRightChecker = 1;    // Check command is received from android (Default : 1, Fail : 0)
    $rpiCmdFetchChekcer = 0;    // Check avr_daemon inserted data to mariadb. (Default : 0, Success : 1, Fail : -1)
    $mariaHost = "localhost";
    $mariaUser = "root";
    $mariaPassword = "raspberry";
    $mariaDBName = "and_rpi_avr";
    $mariaPreData = array('cid' => null);

    $avrBoilerTemp = null;
    $avrDevId = "/dev/ttyUSB0"; // Device location of AVR USB

    function rpiAndCommCheck($andCommGet, $avrBoilerTemp, $rpiCmdFetchChekcer)
    {
        if($rpiCmdFetchChekcer == 0) {
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
                $andCommGet = null;
                $errorCheck = 1;
                return 0;
            }
        } else {
            return 0;
        }
        
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

    function rpiCheckAvrFetch($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $andCommGet, $mariaPreData) /* Need Check on RPi */
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
        
        mysql_close($mariaConnect);
        
        if(($mariaPreData['cid'] != $mariaFetchData['cid']) && ($mariaFetchData['avr_data'] == $andCommGet)) {
            return 1;
        } else {
            return 0;
        }
    }

    function avrSetComm($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $andCommGet, $mariaPreData, $avrBoilerTemp, $avrDevId)
    {
        // Check status before send command to AVR
        $mariaConnect = mysql_connect($mariaHost, $mariaUser, $mariaPassword) or die("MariaDB Connect Error. Please check database name.(From avrSetComm)\n");
        $mariaDBConnect = mysql_select_db($mariaDBName, $mariaConnect);
        mysql_query("set names utf8");
        $mariaQuery = "select * from rpi_avr Order By cid DESC";
        $getMariaData = mysql_query($mariaQuery, $mariaConnect);
        
        $mariaRow = mysql_fetch_array($getMariaData, MYSQL_ASSOC);
        $mariaPreData['cid'] = $mariaRow['cid'];
        
        mysql_close($mariaConnect);
        
        // Send command to AVR
        $avrSendComm = "echo $andCommGet >> $avrDevId";        // Make command to AVR
        exec($avrSendComm);                                    // Send data to AVR
        
        if($avrBoilerTemp != null)
            $avrSendComm = "echo $avrBoilerTemp >> $avrDevId"; // Make boiler temperature command to AVR
        exec($avrSendComm);                                    // Send data to AVR
    }

    // Start Main
    while(1) {
        //$andClientId = $_POST["andClientId"];
        //$andModeGet = $_POST["andModeSet"];
        //$andCommGet = $_POST["avrCommSet"];
        //$avrBoilerTemp = $_POST["avrBoilerSet"];

        // Check client_id between saved raspberry data and received android data
        if($andClientId != $rpiClientId) {
            $andClientId = null;
            $andModeGet = null;
            $andCommGet = null;
            $avrBoilerTemp = null;
            echo("Client id sent from android not matched!\n");
        }

        if($andModeGet != null) {                       // Standby until receive data from android.
            if($andModeGet == 1) {                      // Mode 1 -> Status check mode.
                andGetMaria($mariaHost, $mariaUser, $mariaPassword, $mariaDBName);
                $andModeGet = null;                     // Reset Android Mode
            } else if($andModeGet == 2) {               // Mode 2 -> Android to AVR set command.
                if($rpiCmdFetchChekcer < 0) {           // Process again only fetch checking part.
                    $rpiCmdFetchChekcer = rpiCheckAvrFetch($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $andCommGet, $mariaPreData);
                    if($rpiCmdFetchChekcer == true) {   // When data fetch checked
                        andGetMaria($mariaHost, $mariaUser, $mariaPassword, $mariaDBName);   
                        $andCommGet = null;             // Reset Android command
                        $andModeGet = null;
                        $mariaPreData = null;
                        echo("Command applied successfully.\n");
                    } else {
                        $rpiCmdFetchChekcer = -1;       // Process again only fetch checking part.
                        echo("Command apllied not yet. Check again.(From rpiCmdFetchChekcer < 0)\n");
                    }
                }
                
                $rpiCmdRightChecker = rpiAndCommCheck($andCommGet, $avrBoilerTemp, $rpiCmdFetchChekcer); // Check data correction from android.
                if($rpiCmdRightChecker == true) {
                    avrSetComm($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $andCommGet, $mariaPreData, $avrBoilerTemp, $avrDevId);
                    $rpiCmdFetchChekcer = rpiCheckAvrFetch($mariaHost, $mariaUser, $mariaPassword, $mariaDBName, $andCommGet, $mariaPreData);
                    if($rpiCmdFetchChekcer == true) {   // When data fetch checked
                        andGetMaria($mariaHost, $mariaUser, $mariaPassword, $mariaDBName);   
                        $andCommGet = null;             // Reset Android command
                        $andModeGet = null;
                        $mariaPreData = null;
                        echo("Command applied successfully.\n");
                        $rpiCmdRightChecker = 0;        // Reset Checker
                    } else {
                        $rpiCmdFetchChekcer = -1;       // Process again only fetch checking part.
                        echo("Command apllied not yet. Check again.(From rpiCmdFetchChekcer == true)\n");
                    }
                }
            } else {
                echo("Android mode receive data error!\n");
            }
        }
    }
?>