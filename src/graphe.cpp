//
//  Graphe.cpp
//  Classe pour graphes orientés pondérés (non négativement) avec listes d'adjacence
//
//  Mario Marchand automne 2016.
//

#include "graphe.h"

using namespace std;

//! \brief Constructeur avec paramètre du nombre de sommets désiré
//! \param[in] p_nbSommets indique le nombre de sommets désiré
//! \post crée le vecteur de p_nbSommets de listes d'adjacence vides
Graphe::Graphe(size_t p_nbSommets)
: m_listesAdj(p_nbSommets)
{
}

//! \brief change le nombre de sommets du graphe
//! \param[in] p_nouvelleTaille indique le nouveau nombre de sommet
//! \post le graphe est un vecteur de p_nouvelleTaille de listes d'adjacence
//! \post les anciennes listes d'adjacence sont toujours présentes lorsque p_nouvelleTaille >= à l'ancienne taille
//! \post les dernières listes d'adjacence sont enlevées lorsque p_nouvelleTaille < à l'ancienne taille
void Graphe::resize(size_t p_nouvelleTaille)
{
    m_listesAdj.resize(p_nouvelleTaille);
}

size_t Graphe::getNbSommets() const
{
	return m_listesAdj.size();
}

size_t Graphe::getNbArcs() const
{
    size_t nbArcs = 0;

    for (auto liste : m_listesAdj) {
        nbArcs += liste.size();
    }

    return nbArcs;
}

//! \brief ajoute un arc d'un poids donné dans le graphe
//! \param[in] i: le sommet origine de l'arc
//! \param[in] j: le sommet destination de l'arc
//! \param[in] poids: le poids de l'arc
//! \pre les sommets i et j doivent exister
//! \throws logic_error lorsque le sommet i ou le sommet j n'existe pas
//! \throws logic_error lorsque le poids == numeric_limits<unsigned int>::max()
void Graphe::ajouterArc(size_t i, size_t j, unsigned int poids)
{
    if (i >= m_listesAdj.size()) throw logic_error("Graphe::ajouterArc(): tentative d'ajouter l'arc(i,j) avec un sommet i inexistant");
    if (j >= m_listesAdj.size()) throw logic_error("Graphe::ajouterArc(): tentative d'ajouter l'arc(i,j) avec un sommet j inexistant");
    if (poids == numeric_limits<unsigned int>::max())
        throw logic_error("Graphe::ajouterArc(): valeur de poids interdite");
    m_listesAdj[i].push_back(Arc(j, poids));
}

//! \brief enlève un arc dans le graphe
//! \param[in] i: le sommet origine de l'arc
//! \param[in] j: le sommet destination de l'arc
//! \pre l'arc (i,j) et les sommets i et j dovent exister
//! \post enlève l'arc mais n'enlève jamais le sommet i
//! \throws logic_error lorsque le sommet i ou le sommet j n'existe pas
//! \throws logic_error lorsque l'arc n'existe pas
void Graphe::enleverArc(size_t i, size_t j)
{
    if (i >= m_listesAdj.size()) throw logic_error("Graphe::enleverArc(): tentative d'enlever l'arc(i,j) avec un sommet i inexistant");
    if (j >= m_listesAdj.size()) throw logic_error("Graphe::enleverArc(): tentative d'enlever l'arc(i,j) avec un sommet j inexistant");
    auto & liste = m_listesAdj[i];
    bool arc_enleve = false;
    for (auto itr = liste.end(); itr != liste.begin();) //on débute par la fin par choix
    {
        if ( (--itr)->destination == j )
        {
            liste.erase(itr);
            arc_enleve = true;
            break;
        }
    }
    if (arc_enleve == false) throw logic_error("Graphe::enleverArc: cet arc n'existe pas; donc impossible de l'enlever");
}


unsigned int Graphe::getPoids(size_t i, size_t j) const
{
    if (i >= m_listesAdj.size()) throw logic_error("Graphe::getPoids(): l'incice i n,est pas un sommet existant");
    for (auto itr = m_listesAdj[i].begin(); itr != m_listesAdj[i].end(); ++itr)
    {
        if (itr->destination == j) return itr->poids;
    }
    throw logic_error("Graphe::getPoids(): l'arc(i,j) est existant");
}


bool Graphe::isCyclicUtil(int node, bool visited[], bool *recStack) const
{
    if (!visited[node])
    {
        visited[node] = true;
        recStack[node] = true;

        for (auto it = m_listesAdj[node].begin() ; it != m_listesAdj[node].end() ; ++it)
        {
            if ( !visited[(*it).destination] && isCyclicUtil((*it).destination, visited, recStack) )
                return true;
            else if (recStack[(*it).destination])
                return true;
        }
    }
    recStack[node] = false;
    return false;
}

bool Graphe::isCyclic() const
{

    int nbSommets = m_listesAdj.size();
    bool *visited = new bool[nbSommets];
    bool *recStack = new bool[nbSommets];
    for(int i = 0; i < nbSommets; i++)
    {
        visited[i] = false;
        recStack[i] = false;
    }

    for(int i = 0; i < nbSommets; i++)
        if (isCyclicUtil(i, visited, recStack))
            return true;

        return false;
    }

//! \brief Algorithme de Dijkstra permettant de trouver le plus court chemin entre p_origine et p_destination
//! \pre p_origine et p_destination doivent être des sommets du graphe
//! \return la longueur du plus court chemin est retournée
//! \param[out] le chemin est retourné (un seul noeud si p_destination == p_origine ou si p_destination est inatteignable)
//! \return la longueur du chemin (= numeric_limits<unsigned int>::max() si p_destination n'est pas atteignable)
//! \throws logic_error lorsque p_origine ou p_destination n'existe pas
unsigned int Graphe::plusCourtChemin(size_t p_origine, size_t p_destination, std::vector<size_t> &p_chemin) const
{

    //return defaultDijkstra(p_origine, p_destination, p_chemin);
    if (p_origine >= m_listesAdj.size() || p_destination >= m_listesAdj.size())
        throw logic_error("Graphe::dijkstra(): p_origine ou p_destination n'existe pas");

    p_chemin.clear();

    if (p_origine == p_destination)
    {
        p_chemin.push_back(p_destination);
        return 0;
    }

    vector<unsigned int> distance(m_listesAdj.size(), numeric_limits<unsigned int>::max());
    vector<size_t> predecesseur(m_listesAdj.size(), numeric_limits<size_t>::max());
    distance[p_origine] = 0;

    std::priority_queue<weight_pair, std::vector<weight_pair>, std::greater<weight_pair> > node_queue;
    node_queue.push(std::make_pair(distance[p_origine], p_origine));

    while (!node_queue.empty())
    {
        
        unsigned int dist = node_queue.top().first;
        size_t u = node_queue.top().second;
        node_queue.pop();

        if (u == p_destination) break;

        //noeud obsolète, à ignorer
        if (dist > distance[u]) continue;

        for (auto it = m_listesAdj[u].begin() ; it != m_listesAdj[u].end() ; ++it)
        {
            unsigned int temp = dist + it->poids;
            if (temp < distance[it->destination])
            {

                distance[it->destination] = temp;
                predecesseur[it->destination] = u;
                node_queue.push(std::make_pair(distance[it->destination], it->destination));
            }
        }
    }

    if (predecesseur[p_destination] == numeric_limits<unsigned int>::max())
    {
        p_chemin.push_back(p_destination);
        return numeric_limits<unsigned int>::max();
    }

    size_t dest = p_destination;
    p_chemin.insert(p_chemin.begin(), dest);

    while(dest != numeric_limits<size_t>::max()) {
        dest = predecesseur[dest];
        p_chemin.insert(p_chemin.begin(), dest);
    }

    p_chemin.erase(p_chemin.begin());
    return distance[p_destination];
}


unsigned int Graphe::dijkstraWithSet(size_t p_origine, size_t p_destination, std::vector<size_t> &p_chemin) const
{

    //return defaultDijkstra(p_origine, p_destination, p_chemin);
    if (p_origine >= m_listesAdj.size() || p_destination >= m_listesAdj.size())
        throw logic_error("Graphe::dijkstra(): p_origine ou p_destination n'existe pas");

    p_chemin.clear();

    if (p_origine == p_destination)
    {
        p_chemin.push_back(p_destination);
        return 0;
    }

    vector<unsigned int> distance(m_listesAdj.size(), numeric_limits<unsigned int>::max());
    vector<size_t> predecesseur(m_listesAdj.size(), numeric_limits<size_t>::max());
    distance[p_origine] = 0;

    std::set<weight_pair > node_queue;
    node_queue.insert(std::make_pair(distance[p_origine], p_origine));

    while (!node_queue.empty())
    {

        unsigned int dist = node_queue.begin()->first;
        size_t u = node_queue.begin()->second;
        node_queue.erase(node_queue.begin());

        if (u == p_destination) break;

        for (auto it = m_listesAdj[u].begin() ; it != m_listesAdj[u].end() ; ++it)
        {
            unsigned int temp = dist + it->poids;
            if (temp < distance[it->destination])
            {

                node_queue.erase(std::make_pair(distance[it->destination], it->destination));
                distance[it->destination] = temp;
                predecesseur[it->destination] = u;
                node_queue.insert(std::make_pair(distance[it->destination], it->destination));

            }
        }
    }

    if (predecesseur[p_destination] == numeric_limits<unsigned int>::max())
    {
        p_chemin.push_back(p_destination);
        return numeric_limits<unsigned int>::max();
    }

    size_t dest = p_destination;
    p_chemin.insert(p_chemin.begin(), dest);

    while(dest != numeric_limits<size_t>::max()) {
        dest = predecesseur[dest];
        p_chemin.insert(p_chemin.begin(), dest);
    }

    //cheating, must correct
    p_chemin.erase(p_chemin.begin());
    return distance[p_destination];
}

//! \brief Algorithme de Dijkstra permettant de trouver le plus court chemin entre p_origine et p_destination
//! \pre p_origine et p_destination doivent être des sommets du graphe
//! \return la longueur du plus court chemin est retournée
//! \param[out] le chemin est retourné (un seul noeud si p_destination == p_origine ou si p_destination est inatteignable)
//! \return la longueur du chemin (= numeric_limits<unsigned int>::max() si p_destination n'est pas atteignable)
//! \throws logic_error lorsque p_origine ou p_destination n'existe pas
unsigned int Graphe::defaultDijkstra(size_t p_origine, size_t p_destination, std::vector<size_t> &p_chemin) const
{
    if (p_origine >= m_listesAdj.size() || p_destination >= m_listesAdj.size())
        throw logic_error("Graphe::dijkstra(): p_origine ou p_destination n'existe pas");
    
    p_chemin.clear();
    
    if (p_origine == p_destination)
    {
        p_chemin.push_back(p_destination);
        return 0;
    }
    vector<unsigned int> distance(m_listesAdj.size(), numeric_limits<unsigned int>::max());
    vector<size_t> predecesseur(m_listesAdj.size(), numeric_limits<size_t>::max());
    distance[p_origine] = 0;
    
    list<size_t> q; //ensemble des noeuds non solutionnés;
    for (size_t i = 0; i < m_listesAdj.size(); ++i) //construction de q
    {
        q.push_back(i);
    }
    
    //Boucle principale: touver distance[] et predecesseur[]
    while (!q.empty())
    {
        //trouver uStar dans q tel que distance[uStar] est minimal
        list<size_t>::iterator uStar_itr = q.end();
        unsigned int min = numeric_limits<unsigned int>::max();
        for (auto itr = q.begin(); itr != q.end(); ++itr)
        {
            if (distance[*itr] < min)
            {
                min = distance[*itr];
                uStar_itr = itr;
            }
        }
        if (uStar_itr == q.end()) break; //quitter la boucle : il est impossible de se rendre à destination
        
        size_t uStar = *uStar_itr; //le noeud solutionné
        q.erase(uStar_itr); //l'enlevé de q
        
        if (uStar == p_destination) break; //car on a obtenu distance[p_destination] et predecesseur[p_destination]
        
        //relâcher les arcs sortant de uStar
        for (auto u_itr = m_listesAdj[uStar].begin(); u_itr != m_listesAdj[uStar].end(); ++u_itr)
        {
            unsigned int temp = distance[uStar] + u_itr->poids;
            if (temp < distance[u_itr->destination])
            {
                distance[u_itr->destination] = temp;
                predecesseur[u_itr->destination] = uStar;
            }
        }
    }
    
    //cas où l'on n'a pas de solution
    if (predecesseur[p_destination] == numeric_limits<unsigned int>::max())
    {
        p_chemin.push_back(p_destination);
        return numeric_limits<unsigned int>::max();
    }
    
    //On a une solution, donc construire le plus court chemin à l'aide de predecesseur[]
    stack<size_t> pileDuChemin;
    size_t numero = p_destination;
    pileDuChemin.push(numero);
    while (predecesseur[numero] != numeric_limits<size_t>::max())
    {
        numero = predecesseur[numero];
        pileDuChemin.push(numero);
    }
    while (!pileDuChemin.empty())
    {
        size_t temp = pileDuChemin.top();
        p_chemin.push_back(temp);
        pileDuChemin.pop();
    }
    return distance[p_destination];
}