const WALK_SPEED=80, WHEEL_SPEED=60;
const ICONS={Department:"🎓",Office:"🏛️","Office / Lab":"💻",Library:"📚",Canteen:"🍽️",Mosque:"🕌",Hall:"🎭",Other:"🚪"};
const $=id=>document.getElementById(id);
let LOCATIONS=[], ROUTES={}, GRAPH=[], map, markers={}, routeLine, endpoints, timer=null, userDot=null, userCircle=null;
function toast(message){const t=document.createElement('div');t.className='toast';t.textContent=message;document.body.appendChild(t);setTimeout(()=>t.remove(),3500);}
function compass(a,b){const angle=(Math.atan2(b.lng-a.lng,b.lat-a.lat)*180/Math.PI+360)%360;return ["north","north-east","east","south-east","south","south-west","west","north-west"][Math.round(angle/45)%8];}
function initMap(){
 map=L.map('map').setView([30.22111,71.445],17);
 const light=L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',{maxZoom:19,attribution:'&copy; OpenStreetMap contributors'});
 const dark=L.tileLayer('https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png',{maxZoom:19,attribution:'&copy; OpenStreetMap &copy; CARTO'});light.addTo(map);
 $('themeBtn').onclick=()=>{if(map.hasLayer(light)){map.removeLayer(light);dark.addTo(map);$('themeBtn').textContent='Light map';}else{map.removeLayer(dark);light.addTo(map);$('themeBtn').textContent='Dark map';}};
 routeLine=L.polyline([],{weight:6}).addTo(map);endpoints=L.layerGroup().addTo(map);
 LOCATIONS.forEach(loc=>{const icon=L.divIcon({className:'',html:`<div class="pin"><span>${ICONS[loc.category]||'📍'}</span></div>`,iconSize:[34,34],iconAnchor:[17,34],popupAnchor:[0,-34]});
 const badge=loc.hasRamp?'<span class="badge" style="background:#1a8a4a">Wheelchair accessible</span>':'<span class="badge" style="background:#c0392b">No ramp</span>';
 const popup=`<div class="popup"><b>${loc.name}</b><br><i>${loc.category}</i><div style="margin:6px 0;font-size:13px;line-height:1.4"><b>Block:</b> ${loc.block}<br><b>Floor:</b> ${loc.floor}<br><b>Rooms:</b> ${loc.rooms}<br><b>Raasta:</b> ${loc.route}${loc.note?'<br><i>'+loc.note+'</i>':''}</div>${badge}<br><button onclick="setPoint('start',${loc.id})">Start here</button><button onclick="setPoint('end',${loc.id})">Go here</button></div>`;
 markers[loc.id]=L.marker([loc.lat,loc.lng],{icon}).addTo(map).bindPopup(popup,{maxWidth:270});});
 map.fitBounds(L.latLngBounds(LOCATIONS.map(l=>[l.lat,l.lng])),{padding:[40,40]});
 ['start','end'].forEach((id,i)=>{$(id).innerHTML=`<option value="">${i===0?'Choose start':'Choose destination'}</option>`+LOCATIONS.map(l=>`<option value="${l.id}">${l.name}</option>`).join('');});
 const params=new URLSearchParams(location.search);if(params.has('start')){$('start').value=params.get('start');}
 $('go').onclick=showRoute;$('acc').onchange=()=>{if($('start').value!==''&&$('end').value!=='')showRoute();};
 $('swap').onclick=()=>{const t=$('start').value;$('start').value=$('end').value;$('end').value=t;showRoute();};
 $('locBtn').onclick=getMyLocation;$('search').addEventListener('input',searchLocations);
}
function setPoint(which,id){$(which).value=String(id);map.closePopup();if($('start').value!==''&&$('end').value!=='')showRoute();}
function showRoute(){const s=$('start').value,e=$('end').value,acc=$('acc').checked;$('out').style.display='block';$('msg').textContent='';$('steps').innerHTML='';$('stats').style.display='none';clearInterval(timer);routeLine.setLatLngs([]);endpoints.clearLayers();
 if(s===''||e===''){$('outTitle').textContent='Route';$('msg').textContent='Choose both a start and a destination.';return;}
 if(s===e){$('outTitle').textContent='Route';$('msg').textContent='Start and destination are the same place.';return;}
 const table=acc?ROUTES.accessible:ROUTES.standard,r=table[`${s}-${e}`],from=LOCATIONS[Number(s)],to=LOCATIONS[Number(e)];$('outTitle').textContent=`${from.name} to ${to.name}`;
 if(!r||r.distance<0){$('msg').textContent='No wheelchair accessible route exists between these places. Untick the option to see the standard route.';return;}
 const pts=r.path.map(id=>[LOCATIONS[id].lat,LOCATIONS[id].lng]);routeLine.setStyle({color:acc?'#1a8a4a':getComputedStyle(document.body).getPropertyValue('--route'),dashArray:acc?'10 8':null});map.fitBounds(L.latLngBounds(pts),{padding:[60,60]});
 L.circleMarker(pts[0],{radius:9,color:'#fff',fillColor:'#1a8a4a',fillOpacity:1,weight:3}).addTo(endpoints);L.circleMarker(pts[pts.length-1],{radius:9,color:'#fff',fillColor:'#c0392b',fillOpacity:1,weight:3}).addTo(endpoints);
 const dense=[];for(let i=0;i<pts.length-1;i++)for(let k=0;k<20;k++)dense.push([pts[i][0]+(pts[i+1][0]-pts[i][0])*k/20,pts[i][1]+(pts[i+1][1]-pts[i][1])*k/20]);dense.push(pts[pts.length-1]);let n=0;timer=setInterval(()=>{n+=2;if(n>=dense.length){routeLine.setLatLngs(dense);clearInterval(timer);}else routeLine.setLatLngs(dense.slice(0,n));},25);
 $('dist').textContent=r.distance;$('time').textContent=Math.max(1,Math.round(r.distance/(acc?WHEEL_SPEED:WALK_SPEED)));$('stats').style.display='flex';
 let html='';for(let i=0;i<r.path.length-1;i++){const a=LOCATIONS[r.path[i]],b=LOCATIONS[r.path[i+1]];html+=`<li>Walk ${GRAPH[a.id][b.id]} m ${compass(a,b)} from <b>${a.name}</b> to <b>${b.name}</b>.</li>`;}html+=`<li>You have arrived at <b>${to.name}</b>.</li>`;$('steps').innerHTML=html;
}
function searchLocations(){const q=$('search').value.trim().toLowerCase(),list=$('results');list.innerHTML='';if(!q)return;const hits=LOCATIONS.filter(l=>(l.name+' '+l.category+' '+l.block+' '+l.rooms+' '+l.route+' '+l.note).toLowerCase().includes(q));if(!hits.length){list.innerHTML='<li style="cursor:default">No match found. Try a building or office name.</li>';return;}hits.slice(0,6).forEach(l=>{const li=document.createElement('li');li.innerHTML=`${ICONS[l.category]||'📍'} <b>${l.name}</b><small>${l.category} | ${l.block}</small>`;li.onclick=()=>{map.flyTo([l.lat,l.lng],18);markers[l.id].openPopup();list.innerHTML='';$('search').value='';};list.appendChild(li);});}
function distMeters(a,b,c,d){const dy=(c-a)*111000,dx=(d-b)*111000*Math.cos(a*Math.PI/180);return Math.sqrt(dx*dx+dy*dy);}
function getMyLocation(){if(!navigator.geolocation){toast('This browser does not support location.');return;}toast('Finding your location...');navigator.geolocation.getCurrentPosition(pos=>{const lat=pos.coords.latitude,lng=pos.coords.longitude,accuracy=pos.coords.accuracy;if(userDot)map.removeLayer(userDot);if(userCircle)map.removeLayer(userCircle);userCircle=L.circle([lat,lng],{radius:accuracy,color:'#1d6fd8',weight:1,fillOpacity:.12}).addTo(map);userDot=L.circleMarker([lat,lng],{radius:8,color:'#fff',fillColor:'#1d6fd8',fillOpacity:1,weight:3}).addTo(map).bindPopup('You are here');let nearest=null,best=Infinity;LOCATIONS.forEach(l=>{const d=distMeters(lat,lng,l.lat,l.lng);if(d<best){best=d;nearest=l;}});if(best>600){map.setView([lat,lng],16);toast('You seem to be outside the campus. Come to the Main Gate to start.');return;}$('start').value=nearest.id;map.setView([lat,lng],18);toast(`You are near ${nearest.name} (about ${Math.round(best)} m). Start set. Choose a destination.`);if($('end').value!=='')showRoute();},err=>{const reasons={1:'Location permission denied. Allow location in browser settings.',2:'Location not available. Turn on GPS and try again.',3:'Location took too long. Try again outside.'};toast(reasons[err.code]||'Could not get location.');},{enableHighAccuracy:true,timeout:15000});}
async function startApp(){try{const response=await fetch('campus_data.json');if(!response.ok)throw new Error('campus_data.json not found. First compile and run navigator.cpp.');const data=await response.json();LOCATIONS=data.locations;ROUTES={standard:data.standard,accessible:data.accessible};GRAPH=data.graph||[];if(!Array.isArray(LOCATIONS)||!LOCATIONS.length)throw new Error('Campus data is empty.');initMap();}catch(err){$('map').innerHTML=`<div style="padding:20px;color:#c0392b"><b>Could not load campus data.</b><br>${err.message}<br><br>Run navigator.cpp first to generate campus_data.json, then start a local server.</div>`;}}
startApp();
