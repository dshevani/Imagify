<?php
	header("content-type: application/json");
?>

<?php
$br = "<br>";
$data_url = $_REQUEST['url'];
$data_url = urldecode($data_url);
//echo $data_url;
$keywords = "";
$xpath2 = "//div[contains(@class,\"description\")]/p";
$xpath1 = "//div[contains(@class,\"title\")]/p";
$title = "";
$description = "";
$divid = $_REQUEST['id'];

$up = '123050045:legend4635211$';
$defaultImage = 'http://www.cse.iitb.ac.in/~umangmathur/images/no_image.png';
### proxy for libxml ###

$auth = base64_encode($up);
#echo $auth;
$r_default_context = stream_context_get_default ( array 
                (
                'http' => array( 
                    'proxy' => "netmon.iitb.ac.in:80", 
                    'request_fulluri' => True, 
                    'header' => "Proxy-Authorization: Basic $auth",
                ), 
                'https' => array( 
                    'proxy' => "netmon.iitb.ac.in:80", 
                    'request_fulluri' => True, 
                    'header' => "Proxy-Authorization: Basic $auth",
                ), 
            	) 
        ); 
libxml_set_streams_context($r_default_context); 

### Fetching Title, Descriptions ###

$yql_url = 'http://query.yahooapis.com/v1/public/yql';
$yql_query = "select content from html where url='$data_url' and (xpath='$xpath1' or xpath='$xpath2')";
$yql_query_url = $yql_url . "?q=" . urlencode($yql_query) . "&format=xml";

$ch = curl_init($yql_query_url);
curl_setopt($ch, CURLOPT_URL, $yql_query_url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_PROXY, 'netmon.iitb.ac.in:80');
curl_setopt($ch, CURLOPT_PROXYUSERPWD, $up);

$html = curl_exec($ch);
$xml = simplexml_load_string($html);
$result = $xml->xpath('/query/results/p');
$maxcount = sizeof($result);
if($maxcount<2){
	echo "urlCallback(";
	echo json_encode("{\"id\":\"".$divid."\",\"url\":\"".$defaultImage."\"}");	
	echo ");";
	return;
}
$title = $result[0];
$description = $result[1];

/*
echo "Title=".$title.$br;
echo "Descriptions=".$description.$br;
*/

######  extract Keywords #########################

$keywords = $title;
$keywords = exec("/var/www/hack/senna \"$title\"", $retval);
//echo $keywords;
####### Fetch Imagle List ########################

require("OAuth.php");
 
$cc_key  = "dj0yJmk9bWdiMlVSMWF0a1ptJmQ9WVdrOVEyaFpja05ZTnpJbWNHbzlOVGs1TURRd09UWXkmcz1jb25zdW1lcnNlY3JldCZ4PTRi";
$cc_secret = "d9213c27aa02b4bb35c0fcfcd124fc7ccd47bc1e";
$url = "http://yboss.yahooapis.com/ysearch/images";
$args = array();
$args["q"] = $keywords;
$args["count"] = 5;
$args["format"] = "xml";
 
$consumer = new OAuthConsumer($cc_key, $cc_secret);
$request = OAuthRequest::from_consumer_and_token($consumer, NULL,"GET", $url, $args);
$request->sign_request(new OAuthSignatureMethod_HMAC_SHA1(), $consumer, NULL);
$url = sprintf("%s?%s", $url, OAuthUtil::build_http_query($args));
$ch = curl_init();
$headers = array($request->to_header());
curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_PROXY, 'netmon.iitb.ac.in:80');
curl_setopt($ch, CURLOPT_PROXYUSERPWD, $up);

$rsp = curl_exec($ch);
$html = simplexml_load_string($rsp);
$result = $html->xpath('/bossresponse/images/results/result');
#echo $result; 
#print_r($result);


$ch = curl_init($yql_query_url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_PROXY, 'netmon.iitb.ac.in:80');
curl_setopt($ch, CURLOPT_PROXYUSERPWD, $up);

$yql_url = "http://swoogle.umbc.edu/StsService/GetStsSim?operation=api&phrase1=".urlencode($title);
$maxurl = "";
$maxthumbnailurl = "";
$maxtitle = "";
$maxscore = 0.0;
foreach( $result as $element ){
	$yql_query_url = $yql_url."&phrase2=" . urlencode($element->title);
	curl_setopt($ch, CURLOPT_URL, $yql_query_url);
	$html = curl_exec($ch);
	if($html>$maxscore){
		$maxscore = $html;
		$maxtitle = $element->title;
		$maxurl = $element->url;
		$maxthumbnailurl = $element->thumbnailurl;
	}
/*
	echo $html.$br;
	echo $element->title.$br;
	echo $element->url.$br;
	echo $element->thumbnailurl.$br;
*/
}
if($maxscore<0.40){
	$maxurl = $defaultImage;
}
/*
echo $maxscore.$br;
echo $maxtitle.$br;
echo $maxurl.$br;
echo $maxthumbnailurl.$br;
*/

echo "urlCallback(";
echo json_encode("{\"id\":\"".$divid."\",\"url\":\"".$maxurl."\"}");
echo ");";

?>

