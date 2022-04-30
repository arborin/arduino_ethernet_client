<?php



date_default_timezone_set('Asia/Tbilisi');
// date_default_timezone_set('Asia/Beirut');

require('db.php');


$button_status = [
    'btn_2' => [0 => 'UPS FAULT',               1 => 'UPS Normal'],
    'btn_3' => [0 => 'SELECTOR SWITCH IS OFF',  1 => 'SELECTOR SWITCH IS ON'],
    'btn_5' => [0 => 'EDL IS OFF',              1 => 'EDL IS ON'],
    'btn_6' => [0 => 'GEN IS OFF',              1 => 'GEN IS ON'],
    'btn_7' => [0 => 'SUB. GEN IS OFF',         1 => 'SUB. GEN IS ON']
];


$arduino_ip = $_SERVER['REMOTE_ADDR']; // FOR ACCESS CONTROLL


# FOR BUTTON LOG

if( isset($_GET['btn']) and isset($_GET['name']) and isset($_GET['state'])){
    
    $btn                    = $_GET['btn'];
    $arduino_name           = $_GET['name'];
    $state                  = $_GET['state'];
    
    
    if( array_key_exists($btn, $button_status) ){
        
        
        $btn_vals           = $button_status[$btn];
        $status              = $btn_vals[$state];
        
        $sql                = 'INSERT INTO button_log(arduino_name, status, get_time) VALUES(:arduino_name, :status, now())';

        $statement          = $pdo->prepare($sql);

        $statement->execute([
            ':arduino_name' => $arduino_name,
            ':status'       => $status,
        ]);

        // $log_id             = $pdo->lastInsertId();
    } 
    
    echo                    "#OK";
}

# FOR LIQUID LEVEL
if( isset($_GET['value']) and isset($_GET['name']) ){
    $value                  = $_GET['value'];
    $arduino_name           = $_GET['name'];
    
    # CHECK IF RECORD EXISTS
    $sql                    = 'SELECT COUNT(*) FROM liquid_level where arduino_name=:arduino_name';

    $statement              = $pdo->prepare($sql);
    
    $statement->execute(['arduino_name' => $arduino_name]);
    
    $cnt                    = $statement->fetchColumn();

    
    // if( $cnt == 0 ){
        # IF RECORD EXISTS IN TABLE UPDATE 
        $sql                = 'INSERT INTO liquid_level(arduino_name, value, get_time) VALUES(:arduino_name, :value, now())';
        
        $statement          = $pdo->prepare($sql);
        
        $statement->execute([
            'arduino_name'  => $arduino_name,
            'value'         => $value
        ]);
        
        // $log_id             = $pdo->lastInsertId();

    // }else{
    //     # ELSE CREATE
    //     $sql                = 'UPDATE liquid_level SET value=:value, get_time=now() where arduino_name=:arduino_name';
    //     $statement          = $pdo->prepare($sql);
        
    //     $statement->execute([
    //         'arduino_name'  => $arduino_name,
    //         'value'         => $value
    //     ]);   
    // }
    
    echo                    "#OK";
    

}

?>