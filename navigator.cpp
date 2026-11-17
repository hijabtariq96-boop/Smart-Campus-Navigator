// Smart Campus Navigator - C++ backend
// Compile: g++ -std=c++17 navigator.cpp -o navigator
// Run: ./navigator (Windows: navigator.exe)
// Generates campus_data.json for the web frontend.
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <limits>
#include <iomanip>
using namespace std;

struct Location { int id; string name, category, block, floor, rooms, route, note; bool hasRamp; double lat, lng; };
const int N = 24;
vector<Location> locations = {
  {0, "Department of Computer Science & Technologies", "Department", "New Lab Block / Computing Block", "Ground & 1st Floor", "Chairperson Office, Computer Labs, Classrooms", "Main gate se seedha andar aayein aur central academic area ki taraf jayein (IT Center ke qareeb).", "Faculty of Computing and Emerging Technologies ka hissa.", true, 30.2225, 71.4429},
  {1, "Department of Emerging Computing Technologies", "Department", "New Lab Block / Computing Block", "Ground & 1st Floor", "HOD Office, AI/Data Science Labs, Lecture Rooms", "Main gate se andar aakar central academic zone mein New Lab Block ki taraf jayein.", "CS department aur IT Center ke bilkul sath waqia hai.", true, 30.2226, 71.4431},
  {2, "Department of Accounting, Commerce & Economics", "Department", "Management Sciences Block", "Ground & 1st Floor", "Chairperson Office, Faculty Rooms, Classrooms", "Main gate se dakhil hokar Admin Building ke qareeb Management Block ki taraf jayein.", "Faculty of Management Sciences ke tehat kaam karta hai.", true, 30.2202, 71.4434},
  {3, "Department of Business & Public Administration", "Department", "Management Sciences Block", "Ground & 1st Floor", "HOD Office, Seminar Room, Classrooms", "Main gate se seedha aakar Commerce/Management wing ki taraf mur jayein.", "BBA aur Public Admin ki classes yahan hoti hain.", true, 30.2203, 71.4433},
  {4, "Department of Biosciences and Technology", "Department", "Science Block (Biosciences Wing)", "Ground & 1st Floor", "Chairperson Office, Biotech & Microbiology Labs", "Main gate se andar aakar central lawns ke qareeb Science Blocks ki taraf jayein.", "Research labs aur departmental library mojood hai.", true, 30.2226, 71.4446},
  {5, "Department of Biological & Environmental Sciences", "Department", "Science Block (Botany/Zoology Wing)", "Ground & 1st Floor", "HOD Office, Botany/Zoology Labs, Museum", "Main gate se central lawns ke sath purane Science Blocks ki taraf jayein.", "Tareekhi science labs is wing mein shamil hain.", true, 30.2228, 71.4449},
  {6, "Department of Chemistry", "Department", "Chemistry Block (Science Wing)", "Ground Floor", "Chairperson Office, Organic/Inorganic Labs", "Labs\tMain gate se andar aakar Physics aur Biology blocks ke darmiyan Chemistry Block mein jayein.", "Chemical store aur specialized labs mojood hain.", true, 30.2229, 71.4452},
  {7, "Department of Physics", "Department", "Physics Block (Main Building)", "Ground Floor", "HOD Office, Electronics & Spectroscopy Labs", "Main gate se markazi imarat (Main Building) mein dakhil hon; Physics Block wahin hai.", "1920 se qaim tareekhi department hai.", false, 30.2225, 71.4453},
  {8, "Department of Mathematics & Statistics", "Department", "Science / Main Academic Block", "Ground & 1st Floor", "Chairperson Office, Staff Room, Lecture Halls", "Main gate se Main Building / Science wing ki taraf seedha jayein.", "Departmental library ki sahulat mojood hai.", true, 30.2229, 71.4446},
  {9, "Department of English", "Department", "Arts & Humanities Block (Main Building)", "Ground & 1st Floor", "Chairperson Office, Corpus Center, Classrooms", "Main gate se markazi laal eenton wali building (Arts Wing) ki taraf jayein.", "Yahan Multan ka pehla 'Corpus Center' bhi mojood hai.", true, 30.2217, 71.446},
  {10, "Department of Media, Creative Arts and Global Political Studies", "Department", "Arts & Social Sciences Block", "Ground & 1st Floor", "HOD Office, Media Studio/Labs, Classrooms", "Main gate se Arts & Humanities wing ki taraf central lawns ke qareeb jayein.", "Political Science, IR, aur Media Studies yahan hain.", true, 30.2219, 71.4462},
  {11, "Department of Social & Behavioral Sciences", "Department", "Arts & Social Sciences Block", "Ground & 1st Floor", "Chairperson Office, Psychology Lab, Classrooms", "Main gate se Arts wing mein English aur Political Science departments ke qareeb jayein.", "Sociology aur Psychology ke programs yahan chalte hain.", true, 30.2219, 71.4459},
  {12, "Department of Urdu & Islamic Studies", "Department", "Arts & Humanities Block", "Ground Floor", "HOD Office, Faculty Rooms, Seminar Hall", "Main gate se purani markazi building ke Arts section ki taraf jayein.", "Campus Masjid ke qareeb waqia hai.", false, 30.2216, 71.4462},
  {13, "Vice Chancellor Office", "Office", "Main Admin Building", "Ground Floor", "VC Office, PA to VC, Committee Room", "Bosan Road Main Gate se dakhil hote hi samne markazi Admin Building mein jayein.", "Phone: +92 61 9210174", true, 30.2201, 71.4449},
  {14, "Registrar Office", "Office", "Main Admin Building", "Ground Floor", "Registrar Office, Admin Branch, Establishment", "Main gate se samne Admin Building mein dakhil hon (VC Office ke qareeb).", "Phone: +92 61 9210073", true, 30.2202, 71.4451},
  {15, "Treasurer Office", "Office", "Main Admin Building", "Ground Floor", "Treasurer Office, Accounts & Fee Section", "Main gate se Admin Building mein dakhil hokar Accounts wing ki taraf jayein.", "Fee challan aur financial matters ke liye.", true, 30.2202, 71.4447},
  {16, "Controller of Examinations", "Office", "Main Admin Building(Exam Wing)", "Ground Floor", "Controller Office, Result & Degree Verification", "Main gate se Admin Building ke Exam section ki taraf jayein.", "Exams, roll number slips, aur degrees ke liye.", true, 30.2203, 71.4449},
  {17, "IT Center", "Office / Lab", "New Lab Block", "Ground Floor", "Director IT Office, Server Room, Computer Labs", "Main gate se central academic area ki taraf New Lab Block mein jayein.", "Wi-Fi login aur digital portal support milti hai.", true, 30.2226, 71.4428},
  {18, "Directorate of Press, Media & Publication (PRO)", "Office", "Main Admin Building", "Ground Floor", "PRO Office, Media Coordination Room", "Main gate se Admin Building mein dakhil hokar Admin offices ke sath dekhein.", "University news aur press releases handle karta hai.", true, 30.22, 71.4452},
  {19, "Central Library", "Library", "Central Library Building", "Ground & 1st Floor", "Reading Halls (Male/Female), HEC Digital Lab", "Main gate se andar aakar academic blocks aur lawns ke darmiyan markazi library jayein.", "Subah 8 se shaam 4 baje tak khuli rehti hai.", true, 30.2217, 71.444},
  {20, "Main Canteen / Cafeteria", "Canteen", "Cafeteria & Commercial Corner", "Ground Floor", "Dining Hall, Tuck Shop, Photocopy/Stationery", "Main gate se andar aakar central academic area ke side corner par jayein.", "Photocopy aur composing shop iske bilkul sath hai.", true, 30.2225, 71.4438},
  {21, "Campus Mosque (Masjid)", "Mosque", "Central Mosque Building", "Ground Floor", "Main Prayer Hall, Wuzu Khana, Washrooms", "Main gate se andar aakar central lawns ke qareeb minar/masjid ki taraf jayein.", "Visitors ke liye washrooms dhoondne ki aasan nishani hai.", true, 30.2208, 71.4461},
  {22, "Ghazali Hall / Main Auditorium", "Hall", "Main Building / Central Campus", "Ground Floor", "Ghazali Hall, Main Auditorium Stage", "Main gate se seedha markazi tareekhi building ke beech mein jayein.", "Seminars, workshops, aur events yahan hote hain.", false, 30.2211, 71.445},
  {23, "Main Gate & Parking Areas", "Other", "Bosan Road Entrance & Side Stand", "Ground Level", "Main Security Gate, Student Bike Stand, VIP Parking", "Bosan Road (Gol Bagh ke qareeb) par markazi gate; dakhil hote hi side par parking stand hai.", "Student bike parking fee taqreeban Rs. 30 hai.", true, 30.2191, 71.445},
};

int graph[N][N] = {
  {0, 20, 260, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 140, 90, 0, 0, 0},
  {20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {260, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 200},
  {0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 30, 60, 70, 40, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 130, 90, 0, 0, 0},
  {0, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 170, 0, 0, 0, 0, 0, 25, 30, 40, 0, 0, 0, 0, 0, 0, 0, 0, 100, 130, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 25, 30, 0, 20, 200, 0, 0, 120, 120},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {140, 0, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 200, 0, 0, 0, 0, 0, 0, 100, 0, 120, 0},
  {90, 0, 0, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 130, 0, 0, 0, 120, 0, 0, 0, 0, 0, 120, 0, 110, 0, 220},
  {0, 0, 200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 120, 0, 0, 0, 0, 0, 0, 0, 0, 220, 0},
};

struct Route { int distance; vector<int> path; };
Route dijkstra(int start, int finish, bool accessible) {
  const int INF = numeric_limits<int>::max()/4;
  vector<int> dist(N, INF), parent(N, -1);
  priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
  if (accessible && (!locations[start].hasRamp || !locations[finish].hasRamp)) return {-1, {}};
  dist[start]=0; pq.push({0,start});
  while(!pq.empty()) {
    auto [d,u]=pq.top(); pq.pop(); if(d!=dist[u]) continue; if(u==finish) break;
    for(int v=0; v<N; ++v) {
      int w=graph[u][v]; if(w<=0) continue;
      if(accessible && !locations[v].hasRamp) continue;
      if(d+w<dist[v]) { dist[v]=d+w; parent[v]=u; pq.push({dist[v],v}); }
    }
  }
  if(dist[finish]==INF) return {-1,{}};
  vector<int> path; for(int at=finish; at!=-1; at=parent[at]) path.push_back(at);
  reverse(path.begin(),path.end()); return {dist[finish],path};
}
string escapeJson(const string& s) {
  string out; for(unsigned char c:s) { switch(c) { case '\\': out += "\\\\"; break; case '\"': out += "\\\""; break; case '\n': out += "\\n"; break; case '\r': out += "\\r"; break; case '\t': out += "\\t"; break; default: if(c<32) { const char* hex="0123456789abcdef"; out += "\\u00"; out += hex[c>>4]; out += hex[c&15]; } else out += char(c); } } return out;
}
void writeLocations(ofstream& out) {
  out << "\"locations\":[";
  for(int i=0;i<N;i++) { const auto& l=locations[i]; if(i) out << ',';
    out << "{\"id\":"<<l.id<<",\"name\":\""<<escapeJson(l.name)<<"\",\"category\":\""<<escapeJson(l.category)<<"\",\"block\":\""<<escapeJson(l.block)<<"\",\"floor\":\""<<escapeJson(l.floor)<<"\",\"rooms\":\""<<escapeJson(l.rooms)<<"\",\"route\":\""<<escapeJson(l.route)<<"\",\"note\":\""<<escapeJson(l.note)<<"\",\"hasRamp\":"<<(l.hasRamp?"true":"false")<<",\"lat\":"<<l.lat<<",\"lng\":"<<l.lng<<'}';
  } out << ']';
}
void writeRoutes(ofstream& out, bool accessible) {
  out << (accessible ? "\"accessible\":{" : "\"standard\":{"); bool first=true;
  for(int i=0;i<N;i++) for(int j=0;j<N;j++) if(i!=j) { Route r=dijkstra(i,j,accessible); if(!first) out<<','; first=false;
    out << "\""<<i<<'-'<<j<<"\":{\"distance\":"<<r.distance<<",\"path\":[";
    for(size_t k=0;k<r.path.size();k++){if(k)out<<',';out<<r.path[k];} out<<"]}";
  } out << '}';
}
int main() {
  ofstream out("campus_data.json"); if(!out) { cerr<<"Error: campus_data.json create nahi ho saki.\n"; return 1; }
  out << '{'; writeLocations(out); out << ",\"graph\":["; for(int i=0;i<N;i++){if(i)out<<',';out<<'[';for(int j=0;j<N;j++){if(j)out<<',';out<<graph[i][j];}out<<']';} out << "],"; writeRoutes(out,false); out << ','; writeRoutes(out,true); out << '}'; out.close();
  cout << "campus_data.json successfully generated!\n";
  cout << "Locations: " << N << "\n"; cout << "Now open the web app using a local server.\n"; return 0;
}
