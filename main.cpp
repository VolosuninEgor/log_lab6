#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <string>

struct Graph{
    std::vector<std::vector<int>> matrix;
    std::vector<int> ids;
};

//1.1
void print_matrix(const Graph& g){
    if(g.matrix.empty())return;
    size_t n=g.matrix.size(); 
    std::cout<<std::setw(3)<<" ";
    for(size_t i=0;i<n;++i)std::cout<<std::setw(3)<<g.ids[i];
    std::cout<<"\n";
    for(size_t i=0;i<n;++i){
        std::cout<<std::setw(3)<<g.ids[i];
        for(size_t j=0;j<n;++j){
            std::cout<<std::setw(3)<<g.matrix[i][j];
        }
        std::cout<<"\n";
    }
}

//1.2
void print_list_s(const std::vector<std::vector<int>>& list){
    if(list.empty())return;
    size_t n=list.size(); 
    for(size_t i=0;i<n;++i){
        size_t m=list[i].size();
        for(size_t j=0;j<m;++j){
            std::cout<<std::setw(3)<<list[i][j];
        }
        std::cout<<"\n";
    }
}
    
//1.1
Graph gen_matrix_s(std::mt19937& gen){  
    std::uniform_int_distribution<int> distrib(2,3);
    size_t size=distrib(gen);
    Graph g;
    g.matrix.assign(size,std::vector<int>(size,0));
    g.ids.resize(size);
    for(size_t i=0;i<size;++i)g.ids[i]=static_cast<int>(i);
    std::uniform_int_distribution<int> distrib_b(0,1); 
    for(size_t i=0;i<size;++i){
        for(size_t j=i;j<size;++j){
            g.matrix[i][j]=distrib_b(gen);
            g.matrix[j][i]=g.matrix[i][j];
        }
    }
    return g;
}
    
//1.2
std::vector<std::vector<int>> create_list_s(const Graph& g){
    std::vector<std::vector<int>> list_s;
    size_t n=g.matrix.size();
    for(size_t i=0;i<n;++i){
        std::vector<int> vershina;
        vershina.push_back(g.ids[i]);
        for(size_t j=0;j<n;++j){
            if(g.matrix[i][j])vershina.push_back(g.ids[j]);
        }
        list_s.push_back(vershina);
    }
    return list_s;
}   
   

//2.1
// a 2.1
int find_id_index(const std::vector<int>& v, int id){
    for(size_t i=0; i<v.size();++i){
        if(v[i] == id)return i;
    }
    return -1;
}

Graph identify_matrix(Graph graph,int u,int v){
    int index_u = find_id_index(graph.ids,u); 
    int index_v = find_id_index(graph.ids,v);
    if (index_u==-1||index_v==-1||u==v) return graph;
    int n=graph.matrix.size();

    // ОБРАБОТКА ПЕТЕЛЬ ПРИ ОТОЖДЕСТВЛЕНИИ:
    // по свойству отождествления петля у вершины u остаётся или появляется, если:
    // 1 петля уже была у u, 2 петля была у v, или 3 между u и v было ребро 
    bool has_loop = graph.matrix[index_u][index_u] || graph.matrix[index_v][index_v] || graph.matrix[index_u][index_v];

    for (int j=0;j<n;++j){
        graph.matrix[index_u][j]=graph.matrix[index_u][j]||graph.matrix[index_v][j];
        graph.matrix[j][index_u]=graph.matrix[j][index_u]||graph.matrix[j][index_v]; 
    }
    graph.matrix[index_u][index_u] = has_loop ? 1 : 0;

    graph.matrix.erase(graph.matrix.begin()+index_v);    
    for(auto& row:graph.matrix){
        row.erase(row.begin()+index_v);
    }
    graph.ids.erase(graph.ids.begin()+index_v);

    return graph;
}
// b 2.1
Graph contraction_matrix(Graph graph,int u,int v){
    int index_u = find_id_index(graph.ids,u); 
    int index_v = find_id_index(graph.ids,v);
    if (index_u==-1||index_v==-1||u==v) return graph;
    else if (graph.matrix[index_u][index_v]+graph.matrix[index_v][index_u]==2){ 
        graph = identify_matrix(graph, u, v);
        index_u = find_id_index(graph.ids,u);
        // ОБРАБОТКА ПЕТЕЛЬ ПРИ СТЯГИВАНИИ:
        // по свойству стягивания ребра стянутое ребро исчезает и петля на диагонали удаляется
        graph.matrix[index_u][index_u] = 0;
    }
    return graph;
}
//c 2.1
Graph split_vershin(Graph graph,int u){
    int index_u=find_id_index(graph.ids,u);
    if (index_u==-1)return graph;
    size_t n = graph.matrix.size();
    size_t cnt=0;
    graph.ids.push_back(graph.ids.back()+1);
    std::vector<int> str;
    for (size_t i=0;i<n;++i) {
        // ОБРАБОТКА ПЕТЕЛЬ ПРИ РАСЩЕПЛЕНИИ:
        // собственную петлю вершины u (i == index_u) не делим как обычного соседа, она остаётся у u
        if (i == static_cast<size_t>(index_u)) {
            graph.matrix[i].push_back(0);
        }
        else if (graph.matrix[i][index_u]==1) {
            if(cnt%2) {
                graph.matrix[i].push_back(1);
                graph.matrix[i][index_u]=0;
                graph.matrix[index_u][i] = 0;  
            }
            else graph.matrix[i].push_back(0);
            ++cnt;
        } 
        else graph.matrix[i].push_back(0);
        str.push_back(graph.matrix[i].back());
    }
    graph.matrix[index_u].back() = 1;
    str[index_u] = 1;
    str.push_back(0);
    graph.matrix.push_back(str);
    return graph;
} 

//2.2
int find_list_index(const std::vector<std::vector<int>>& list, int id){
    for(size_t i=0;i<list.size();++i){
        if(list[i][0]==id)return i;
    }
    return -1;
}

bool communication_check(const std::vector<int>& row, int nb){
    for(size_t j=1;j<row.size();++j){
        if(row[j]==nb)return true;
    }
    return false;
}

// a 2.2
std::vector<std::vector<int>> identify_list(std::vector<std::vector<int>> list,int u,int v){
    int index_u = find_list_index(list,u);
    int index_v = find_list_index(list,v);
    if (index_u==-1||index_v==-1||u==v) return list;

    // ОБРАБОТКА ПЕТЕЛЬ ПРИ ОТОЖДЕСТВЛЕНИИ:
    // если у v была петля (nb == v), переводим её на u; если между u и v было ребро (nb == u),
    // u добавится в свой же список соседей и образует петлю
    for (size_t j=1;j<list[index_v].size();++j){
        int nb = list[index_v][j];
        if (nb==v) nb = u;
        if (!communication_check(list[index_u],nb)){
            list[index_u].push_back(nb);
        }
    }
    list.erase(list.begin()+index_v);

    for (auto& row:list){
        for (size_t j=1;j<row.size();++j){
            if (row[j]==v){
                if (row[0]==u || communication_check(row,u)){
                    row.erase(row.begin()+j);
                    --j;
                }
                else row[j]=u;
            }
        }
    }
    return list;
}

// b 2.2
std::vector<std::vector<int>> contraction_list(std::vector<std::vector<int>> list,int u,int v){
    int index_u = find_list_index(list,u);
    int index_v = find_list_index(list,v);
    if (index_u==-1||index_v==-1||u==v) return list;
    else if (communication_check(list[index_u],v)){
        list = identify_list(list, u, v);
        index_u = find_list_index(list,u);
        // ОБРАБОТКА ПЕТЕЛЬ ПРИ СТЯГИВАНИИ:
        // по свойству стягивания ребра удаляем саму вершину u из списка её соседей
        for (size_t j=1;j<list[index_u].size();++j){
            if (list[index_u][j]==u){
                list[index_u].erase(list[index_u].begin()+j);
                break;
            }
        }
    }
    return list;
}

// c 2.2
std::vector<std::vector<int>> split_vershin_list(std::vector<std::vector<int>> list,int u){
    int index_u = find_list_index(list,u);
    if (index_u==-1)return list;
    size_t cnt=0;
    int new_id = list.back()[0]+1;
    std::vector<int> str = {new_id};

    for (size_t j=1;j<list[index_u].size();++j){
        // ОБРАБОТКА ПЕТЕЛЬ ПРИ РАСЩЕПЛЕНИИ:
        // петлю самой вершины u пропускаем, чтобы она осталась у u и не дублировалась
        if (list[index_u][j]==u) continue;
        if (cnt%2){
            int nb = list[index_u][j];
            str.push_back(nb);
            int index_nb = find_list_index(list,nb);
            for (size_t k=1;k<list[index_nb].size();++k){
                if (list[index_nb][k]==u) list[index_nb][k]=new_id;
            }
            list[index_u].erase(list[index_u].begin()+j);
            --j;
        }
        ++cnt;
    }
    list[index_u].push_back(new_id);
    str.push_back(u);
    list.push_back(str);
    return list;
}


//3.1
//a 3.2 обьединение
Graph union_matrix(const Graph& g1, const Graph& g2){
    Graph res;
    res.ids = g1.ids;
    for (int id : g2.ids){
        if (find_id_index(res.ids, id) == -1) res.ids.push_back(id);
    }
    size_t n = res.ids.size();
    res.matrix.assign(n, std::vector<int>(n, 0));

    for (size_t i = 0; i < n; ++i){
        for (size_t j = i; j < n; ++j){
            int u = res.ids[i];
            int v = res.ids[j];
            int i1 = find_id_index(g1.ids, u);
            int j1 = find_id_index(g1.ids, v);
            int i2 = find_id_index(g2.ids, u);
            int j2 = find_id_index(g2.ids, v);

            bool e1 = (i1 != -1 && j1 != -1 && g1.matrix[i1][j1] == 1);
            bool e2 = (i2 != -1 && j2 != -1 && g2.matrix[i2][j2] == 1);

            if (e1 || e2){
                res.matrix[i][j] = 1;
                res.matrix[j][i] = 1;
            }
        }
    }
    return res;
}

//b 3.1 пересечение
Graph intersect_matrix(const Graph& g1, const Graph& g2){
    Graph res;
    for (int id : g1.ids){
        if (find_id_index(g2.ids, id) != -1) res.ids.push_back(id);
    }
    size_t n = res.ids.size();
    res.matrix.assign(n, std::vector<int>(n, 0));

    for (size_t i = 0; i < n; ++i){
        for (size_t j = i; j < n; ++j){
            int u = res.ids[i];
            int v = res.ids[j];
            int i1 = find_id_index(g1.ids, u);
            int j1 = find_id_index(g1.ids, v);
            int i2 = find_id_index(g2.ids, u);
            int j2 = find_id_index(g2.ids, v);

            if (g1.matrix[i1][j1] == 1 && g2.matrix[i2][j2] == 1){
                res.matrix[i][j] = 1;
                res.matrix[j][i] = 1;
            }
        }
    }
    return res;
}

//c
// c. кольцевая сумма графов G = G1 ⊕ G2
Graph ring_sum_matrix(const Graph& g1, const Graph& g2){
    Graph temp;
    temp.ids = g1.ids;
    for (int id : g2.ids){
        if (find_id_index(temp.ids, id) == -1) temp.ids.push_back(id);
    }
    size_t n = temp.ids.size();
    temp.matrix.assign(n, std::vector<int>(n, 0));

    for (size_t i = 0; i < n; ++i){
        for (size_t j = i; j < n; ++j){
            int u = temp.ids[i];
            int v = temp.ids[j];
            int i1 = find_id_index(g1.ids, u);
            int j1 = find_id_index(g1.ids, v);
            int i2 = find_id_index(g2.ids, u);
            int j2 = find_id_index(g2.ids, v);

            bool e1 = (i1 != -1 && j1 != -1 && g1.matrix[i1][j1] == 1);
            bool e2 = (i2 != -1 && j2 != -1 && g2.matrix[i2][j2] == 1);

            if (e1 != e2){
                temp.matrix[i][j] = 1;
                temp.matrix[j][i] = 1;
            }
        }
    }

    // удаление изолированных вершин по определению кольцевой суммы
    Graph res;
    for (size_t i = 0; i < n; ++i){
        bool has_edge = false;
        for (size_t j = 0; j < n; ++j){
            if (temp.matrix[i][j] == 1){
                has_edge = true;
                break;
            }
        }
        if (has_edge) res.ids.push_back(temp.ids[i]);
    }

    size_t k = res.ids.size();
    res.matrix.assign(k, std::vector<int>(k, 0));
    for (size_t i = 0; i < k; ++i){
        for (size_t j = i; j < k; ++j){
            int orig_i = find_id_index(temp.ids, res.ids[i]);
            int orig_j = find_id_index(temp.ids, res.ids[j]);
            res.matrix[i][j] = temp.matrix[orig_i][orig_j];
            res.matrix[j][i] = res.matrix[i][j];
        }
    }
    return res;
}

// 4
Graph dekart_matrix(const Graph& g1, const Graph& g2){
    size_t n1 = g1.matrix.size();
    size_t n2 = g2.matrix.size();
    size_t total = n1 * n2;

    Graph res;
    for (size_t i = 0; i < total; ++i){
        res.ids.push_back(static_cast<int>(i));
    }
    res.matrix.assign(total, std::vector<int>(total, 0));

    for (size_t i = 0; i < n1; ++i){
        for (size_t k = 0; k < n2; ++k){
            size_t p = i * n2 + k;

            for (size_t j = 0; j < n1; ++j){
                for (size_t l = 0; l < n2; ++l){
                    size_t q = j * n2 + l;

                    // Если у вершины i в G1 или у вершины k в G2 есть петля,
                    // при p == q условие автоматически даст 1 на диагонали (петля сохранится)
                    if ((i == j && g2.matrix[k][l] == 1) || (k == l && g1.matrix[i][j] == 1)){
                        res.matrix[p][q] = 1;
                    }
                }
            }
        }
    }
    return res;
}

void print_dekart_matrix(const Graph& g_cart, const Graph& g1, const Graph& g2){
    if(g_cart.matrix.empty())return;
    size_t n1 = g1.ids.size();
    size_t n2 = g2.ids.size();
    size_t n = g_cart.matrix.size();

    std::cout<<std::setw(3)<<" ";
    for(size_t i=0;i<n1;++i){
        for(size_t k=0;k<n2;++k){
            std::cout<<std::setw(3)<<(std::to_string(g1.ids[i])+std::to_string(g2.ids[k]));
        }
    }
    std::cout<<"\n";

    for(size_t i=0;i<n1;++i){
        for(size_t k=0;k<n2;++k){
            size_t p = i * n2 + k;
            std::cout<<std::setw(3)<<(std::to_string(g1.ids[i])+std::to_string(g2.ids[k]));
            for(size_t q=0;q<n;++q){
                std::cout<<std::setw(3)<<g_cart.matrix[p][q];
            }
            std::cout<<"\n";
        }
    }
}

int main(){
    std::random_device rd;
    std::mt19937 gen(rd());
    //1.1
    std::cout<<"task 1.1:\n";
    Graph M1=gen_matrix_s(gen);
    Graph M2=gen_matrix_s(gen);
    std::cout<<"матрица M1:\n";
    print_matrix(M1);
    std::cout<<"матрица M2:\n";
    print_matrix(M2);
    //1.2
    std::cout<<"\ntask 1.2:\n";
    std::vector<std::vector<int>> list_s1=create_list_s(M1);
    std::vector<std::vector<int>> list_s2=create_list_s(M2);
    std::cout<<"список смежности по M1:\n";
    print_list_s(list_s1);
    std::cout<<"список смежности по M2:\n";
    print_list_s(list_s2);

   //2.1, 2.2
   bool input = true; 
   std::cout<<"\ntask 2.1, 2.2:\n";
   while (input) {
       std::cout<<"выполнить:         \n"
           "  a. отождествление вершин\n"
           "  b. стягивание ребра     \n"
           "  c. расщепление вершины  \n";
       char choice;
       std::cout<<"ввод: ";
       std::cin>>choice;
       while (choice!='a'&&choice!='b'&&choice!='c')std::cin>>choice;
       int num;
       std::cout<<"номер матрицы:\n";
       std::cin>>num;
       while (num!=1&&num!=2) std::cin>>num;
       --num;
       switch(choice){
           case 'a': {
               int u, v;
               std::cout<<"введите 2 вершины для отождествления: ";
               std::cin>>u>>v;
               Graph identify_graph=(num?identify_matrix(M2,u,v):identify_matrix(M1,u,v));
               while (identify_graph.matrix.size()==(num?M2.matrix.size():M1.matrix.size())) {
                   std::cout<<"некорректные вершины\a        \n"
                       "введите 2 вершины для отождествления:\n";
                   std::cin>>u>>v;      
                   identify_graph=(num?identify_matrix(M2, u, v):identify_matrix(M1,u,v)); 
               }
               std::cout<<"матрица:\n";
               print_matrix(identify_graph);
               std::vector<std::vector<int>> identify_l=(num?identify_list(list_s2,u,v):identify_list(list_s1,u,v));
               std::cout<<"список смежности:\n";
               print_list_s(identify_l);
               break;
           }
           case 'b': {
               int u, v;
               std::cout<<"введите 2 вершины для стягивания ребра: ";
               std::cin>>u>>v;
               Graph contraction_graph=(num?contraction_matrix(M2,u,v):contraction_matrix(M1,u,v));
               while (contraction_graph.matrix.size()==(num?M2.matrix.size():M1.matrix.size())) {
                   std::cout<<"некорректные вершины\a    \n"
                       "введите 2 вершины для стягивания:\n";
                   std::cin>>u>>v;
                   contraction_graph=(num?contraction_matrix(M2, u, v):contraction_matrix(M1,u,v));
               }
               std::cout<<"матрица:\n";
               print_matrix(contraction_graph);
               std::vector<std::vector<int>> contraction_l=(num?contraction_list(list_s2,u,v):contraction_list(list_s1,u,v));
               std::cout<<"список смежности:\n";
               print_list_s(contraction_l);
               break;
           }
           case 'c': {
               int u;
               std::cout<<"введите вершину для расщепления: ";
               std::cin>>u; 
               Graph split_graph=(num?split_vershin(M2,u):split_vershin(M1,u));
               while (split_graph.matrix.size()==(num?M2.matrix.size():M1.matrix.size())) {
                   std::cout<<"некорректная вершинa\a        \n"
                       "введите вершину для расщепления: ";
                   std::cin>>u;
                   split_graph=(num?split_vershin(M2, u):split_vershin(M1,u));
               }
               std::cout<<"матрица:\n";
               print_matrix(split_graph);
               std::vector<std::vector<int>> split_l=(num?split_vershin_list(list_s2,u):split_vershin_list(list_s1,u));
               std::cout<<"список смежности:\n";
               print_list_s(split_l);
               break;
           }
       }
       //3.1
       std::cout<<"\ntask 3.1:\n";
       std::cout<<"объединение G1 G2:\n";
       Graph G_union = union_matrix(M1, M2);
       print_matrix(G_union);

       std::cout<<"\nпересечение G1 G2:\n";
       Graph G_intersect = intersect_matrix(M1, M2);
       print_matrix(G_intersect);

       std::cout<<"\nкольцевая сумма G1 G2:\n";
       Graph G_ring = ring_sum_matrix(M1, M2);
       print_matrix(G_ring);

        // 4
        std::cout<<"\ntask 4:\n";
        std::cout<<"декартово произведение G1 x G2:\n";
        Graph G_dekart = dekart_matrix(M1, M2);
        print_dekart_matrix(G_dekart, M1, M2);
        

        std::cout<<"\nзавершить работу 1\nпродолжить работу 2:\n";
        char ch;
        std::cin>>ch;
        input = (ch=='2');
    }
}
