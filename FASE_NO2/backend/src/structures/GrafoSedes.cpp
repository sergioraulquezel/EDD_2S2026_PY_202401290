#include "structures/GrafoSedes.h"
#include <fstream>
#include <cstdlib>
#include <set>
#include <iterator>
#include <queue>
#include <algorithm>
bool GrafoSedes::agregarSede(const Sede&s){return !s.codigo.empty()&&sedes.emplace(s.codigo,s).second;}
bool GrafoSedes::existeSede(const std::string&c)const{return sedes.count(c)>0;}
void GrafoSedes::reconstruir(const std::vector<FuncionCine>&fs){adyacencia.clear();for(auto&p:sedes)adyacencia[p.first];std::map<std::string,std::set<std::string>>m;for(auto&f:fs)if(!f.sede.empty()&&existeSede(f.sede)&&f.pelicula)m[f.sede].insert(f.pelicula->getCodigo());for(auto i=m.begin();i!=m.end();++i)for(auto j=std::next(i);j!=m.end();++j){int n=0;for(auto&p:i->second)if(j->second.count(p))++n;if(n){adyacencia[i->first][j->first]=n;adyacencia[j->first][i->first]=n;}}}
int GrafoSedes::peso(const std::string&a,const std::string&b)const{auto i=adyacencia.find(a);if(i==adyacencia.end())return 0;auto j=i->second.find(b);return j==i->second.end()?0:j->second;}
std::vector<AristaSede> GrafoSedes::vecinos(const std::string&c)const{std::vector<AristaSede>v;auto i=adyacencia.find(c);if(i!=adyacencia.end())for(auto&p:i->second)v.push_back({p.first,p.second});return v;}
std::vector<std::string> GrafoSedes::caminoBFS(const std::string& origen,const std::string& destino)const{
 if(!existeSede(origen)||!existeSede(destino))return {};
 std::queue<std::string> pendientes;std::map<std::string,std::string> padre;std::set<std::string> visitados;
 pendientes.push(origen);visitados.insert(origen);
 while(!pendientes.empty()){auto actual=pendientes.front();pendientes.pop();if(actual==destino)break;for(const auto& vecino:vecinos(actual))if(visitados.insert(vecino.destino).second){padre[vecino.destino]=actual;pendientes.push(vecino.destino);}}
 if(!visitados.count(destino))return {};
 std::vector<std::string> camino;for(std::string actual=destino;;actual=padre[actual]){camino.push_back(actual);if(actual==origen)break;}
 std::reverse(camino.begin(),camino.end());return camino;
}
std::vector<RelacionSedes> GrafoSedes::obtenerRelacionesOrdenadas()const{
 std::vector<RelacionSedes> relaciones;for(const auto& a:adyacencia)for(const auto& b:a.second)if(a.first<b.first)relaciones.push_back({a.first,b.first,b.second});
 std::sort(relaciones.begin(),relaciones.end(),[](const RelacionSedes& a,const RelacionSedes& b){if(a.peso!=b.peso)return a.peso>b.peso;if(a.origen!=b.origen)return a.origen<b.origen;return a.destino<b.destino;});return relaciones;
}
void GrafoSedes::generarReporteGraphviz(const std::string&d,const std::string&p)const{std::ofstream f(d);if(!f)return;f<<"graph G { rankdir=LR; node [shape=circle,style=filled,fillcolor=\"#9bd3f5\"];";for(auto&s:sedes)f<<" \""<<s.first<<"\" [label=\""<<s.first<<"\\n"<<s.second.nombre<<"\"];";for(auto&a:adyacencia)for(auto&b:a.second)if(a.first<b.first)f<<" \""<<a.first<<"\" -- \""<<b.first<<"\" [label=\""<<b.second<<"\",color=\"#f28c28\",penwidth=2];";f<<" }";f.close();std::system((std::string("dot -Tpng \"")+d+"\" -o \""+p+"\"").c_str());}