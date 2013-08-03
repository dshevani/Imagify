// ==UserScript==
// @name        TestScript
// @namespace   Piyush
// @include     http://news.yahoo.com/
// @version     1
// @require       http://ajax.googleapis.com/ajax/libs/jquery/1.2.6/jquery.js
// ==/UserScript==

var arr;
var cnt = 0;
var img_url_response, data_uuid_response;

function getUrl(url) {
	var scriptEl = document.createElement("script");
	scriptEl.src = url;
	scriptEl.async = true;
	console.log(document.getElementsByTagName("head"));
	console.log(document.getElementsByTagName("head")[0]);
	document.getElementsByTagName("head")[0].appendChild(scriptEl);
}

window.urlCallback = function (responseData) {
	parseJSONurls = responseData;
	var obj = JSON.parse(responseData);
	//alert(obj.id);
	//alert(obj.url);
	var li_item = $("[data-uuid=" + obj.id + "]")[0];
	 li_item.style.backgroundColor="#E7CCFF"; 
	//li_item.style.background-color = "#d8bfd8";
	var divlevel1 = li_item.getElementsByTagName("div")[0].innerHTML;
	
	divlevel1 = '<a class=\"img ov-h thumb-left  \" href=\"' + obj.url + '\" data-action-outcome=\"navigate\" data-ylk=\"sec:td-strm;ed:0;bpos:1;pos:1; cpos:6;g:b4712847-f81d-35b8-9489-342ab672b0af;tar:news.yahoo.com;ltxt:A-Rodhom; ct:1;pkgt:4;r:4000000595;slk:title;itc:0;prov:AssociatedPress;sort:1; test:Precog_On;intl:us;cat:default;ccode:news;\" tabindex=\"-1\"  data-rapid_p=\"26\"><img src=\"' + obj.url + '\" style=\"\" width=\"82\" height=\"100\" alt=\"\" title=\"\" class=\"\"></a>' + divlevel1;
        li_item.getElementsByTagName("div")[0].innerHTML = divlevel1;
	
};

function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if ((new Date().getTime() - start) > milliseconds){
      break;
    }
  }
}

function getNotNullArray(){
    cnt++;
    console.log(cnt);
	arr=document.getElementsByClassName("voh-parent");
    if(arr.length==0) {
    	sleep(1000);
        getNotNullArray();
    }
}

function imagify(){
arr=document.getElementsByClassName("voh-parent");


console.log(arr);

//alert("Hi");

cnt = 0;
var server_url = "http://10.105.15.59/hack/index.php?id=";
var thestringurl = "&url=";
var li_id, data_url, final_url;

for (var i=0;i<arr.length;i++) {
	    console.log("iter");
	    console.log(arr[i].classList);
	    if(arr[i].classList.contains("has-image")){
	    	console.log("No");
	    }
	    else{
	    	arr[i].classList.add("has-image");
		var divlevel1 = arr[i].getElementsByTagName("div")[0].innerHTML;
		li_id = arr[i].getAttribute("data-uuid");
		data_url = arr[i].getAttribute("data-url");
	
		
	       	final_url = server_url + li_id + thestringurl + data_url;
	       	getUrl(final_url);
		}
		
	} 
}

window.onscroll = imagify;

imagify();


