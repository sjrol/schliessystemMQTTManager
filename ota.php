<?PHP
define(VERSION, 1016);

header('Content-type: text/plain; charset=utf8', true);

function check_header($name, $value = false) {
    if(!isset($_SERVER[$name])) {
        return false;
    }
    if($value && $_SERVER[$name] != $value) {
        return false;
    }
    return true;
}

function sendFile($path) {
    header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
    header('Content-Type: application/octet-stream', true);
    header('Content-Disposition: attachment; filename='.basename($path));
    header('Content-Length: '.filesize($path), true);
    header('x-MD5: '.md5_file($path), true);
    readfile($path);
}

if(!check_header('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater!\n";
    exit();
}

if(
    !check_header('HTTP_X_ESP8266_STA_MAC') ||
    !check_header('HTTP_X_ESP8266_AP_MAC') ||
    !check_header('HTTP_X_ESP8266_FREE_SPACE') ||
    !check_header('HTTP_X_ESP8266_SKETCH_SIZE') ||
    !check_header('HTTP_X_ESP8266_CHIP_SIZE') ||
    !check_header('HTTP_X_ESP8266_SDK_VERSION') ||
    !check_header('HTTP_X_ESP8266_VERSION')
) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater! (header)\n";
    exit();
}

/*
 * $_SERVER['HTTP_X_ESP8266_STA_MAC']
 * $_SERVER['HTTP_X_ESP8266_VERSION']
 */

$release = json_decode(file_get_contents('https://api.github.com/repos/sjrol/schliessystemMQTTManager/releases/latest'));

if(isset($db[$_SERVER['HTTP_X_ESP8266_STA_MAC']])) {
    if($release['name'] != $_SERVER['HTTP_X_ESP8266_VERSION']) {
	foreach($release['assets'] as $asset) {
		if($asset['name'] == 'arduino.bin') {
			$file = file_get_contents($assets['browser_download_url']);
			header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
			header('Content-Type: application/octet-stream', true);
			header('Content-Disposition: attachment; filename='.$asset['name']);
			header('Content-Length: '.$asset['size'], true);
			header('x-MD5: '.md5($file), true);
			echo($file);
		}
	}
    } else {
        header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
    }
    exit();
}

header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);
