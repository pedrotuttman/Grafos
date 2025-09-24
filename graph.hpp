#pragma once
#include <bits/stdc++.h>
using namespace std;

struct Graph {
    // config
    enum Rep { LIST, MATRIX };

    // data
    int n = 0;            // Vértices numerados de 1..n
    long long m = 0;      // número de arestas (não-direcionadas)
    Rep rep = LIST;

    // lista
    vector<vector<int>> adj;

    // matriz (armazenada linear para economia)
    vector<uint8_t> mat;  // n*n, 0/1

    Graph() = default;

    static Graph loadFromFile(const string& path, Rep r) {
        // Lê arquivo com arestas "u v" por linha; ignora linhas vazias e comentários (#)
        // Se a primeira linha contiver dois inteiros, tratamos como "n E" (mas E é ignorado)
        // Caso a primeira linha tenha só um inteiro, tratamos como "n" também.
        ifstream in(path);
        if (!in) throw runtime_error("Não consegui abrir: " + path);

        vector<pair<int,int>> edges;
        edges.reserve(1<<20);
        string line;
        int n_guess = 0;

        auto is_comment = [](const string& s){
            for (char c: s) { if (!isspace((unsigned char)c)) return c=='#'; }
            return false;
        };

        // Primeiro pass: tentar detectar n e coletar arestas
        bool header_processed = false;
        while (getline(in, line)) {
            if (line.empty() || is_comment(line)) continue;
            stringstream ss(line);
            vector<long long> vals;
            long long x;
            while (ss >> x) vals.push_back(x);
            if (!header_processed && (vals.size()==1 || vals.size()==2)) {
                // Pode ser um cabeçalho: n ou n E
                if (vals[0] > 0 && vals[0] < (1<<30)) {
                    n_guess = (int)vals[0];
                    header_processed = true;
                    continue;
                }
            }
            if (vals.size() >= 2) {
                int u = (int)vals[0], v=(int)vals[1];
                if (u>0 && v>0) edges.emplace_back(u,v);
                n_guess = max(n_guess, max(u,v));
            }
        }
        in.close();

        Graph G;
        G.n = n_guess;
        G.rep = r;

        if (G.n <= 0) throw runtime_error("Número de vértices inválido (n <= 0).");

        // Montar lista com deduplicação (sem structured bindings)
        vector<vector<int>> tmp(G.n+1);
        for (const auto& e : edges) {
            int u = e.first;
            int v = e.second;
            if (u==v) continue;
            if (u<1 || u>G.n || v<1 || v>G.n) continue;
            tmp[u].push_back(v);
            tmp[v].push_back(u);
        }
        // ordenar/unique + contar m
        long long sumdeg = 0;
        for (int i=1;i<=G.n;++i){
            auto &vec = tmp[i];
            sort(vec.begin(), vec.end());
            vec.erase(unique(vec.begin(), vec.end()), vec.end());
            sumdeg += (long long)vec.size();
        }
        G.m = sumdeg/2;

        // construir conforme representação
        if (r==LIST){
            G.adj = move(tmp);
        } else {
            // matriz n x n (0/1)
            G.mat.assign((size_t)G.n*(size_t)G.n, (uint8_t)0);
            for (int u=1;u<=G.n;++u){
                for (int v: tmp[u]){
                    G.mat[(size_t)(u-1)*G.n + (size_t)(v-1)] = 1;
                    G.mat[(size_t)(v-1)*G.n + (size_t)(u-1)] = 1;
                }
            }
        }
        return G;
    }

    inline bool hasEdgeMat(int u, int v) const {
        return mat[(size_t)(u-1)*n + (size_t)(v-1)] != 0;
    }

    // Grau / vizinhos (uniformizar acesso)
    inline const vector<int>& neighborsList(int u) const { return adj[u]; }

    vector<int> neighborsMat(int u) const {
        vector<int> out;
        out.reserve(64);
        size_t base = (size_t)(u-1)*n;
        for (int v=1; v<=n; ++v) {
            if (mat[base + (size_t)(v-1)]) out.push_back(v);
        }
        return out;
    }

    // BFS tree
    pair<vector<int>, vector<int>> bfsTree(int s) const {
        vector<int> parent(n+1,-1), level(n+1,-1);
        deque<int> q;
        parent[s]=0; level[s]=0; q.push_back(s);

        if (rep==LIST) {
            while(!q.empty()){
                int u=q.front(); q.pop_front();
                for (int v: neighborsList(u)){
                    if (level[v]==-1){
                        parent[v]=u; level[v]=level[u]+1;
                        q.push_back(v);
                    }
                }
            }
        } else {
            while(!q.empty()){
                int u=q.front(); q.pop_front();
                size_t base = (size_t)(u-1)*n;
                for (int v=1; v<=n; ++v){
                    if (!mat[base + (size_t)(v-1)]) continue;
                    if (level[v]==-1){
                        parent[v]=u; level[v]=level[u]+1;
                        q.push_back(v);
                    }
                }
            }
        }

        return {parent, level};
    }

    // DFS (iterativa) para gerar árvore
    pair<vector<int>, vector<int>> dfsTree(int s) const {
        vector<int> parent(n+1,-1), level(n+1,-1);
        vector<int> it(n+1,0);
        vector<int> st; st.reserve(n);
        parent[s]=0; level[s]=0; st.push_back(s);

        if (rep==LIST) {
            while(!st.empty()){
                int u = st.back();
                auto &N = adj[u];
                if (it[u] < (int)N.size()){
                    int v = N[it[u]++];
                    if (level[v]==-1){
                        parent[v]=u; level[v]=level[u]+1;
                        st.push_back(v);
                    }
                } else {
                    st.pop_back();
                }
            }
        } else {
            // usar vetor de vizinhos gerado dinamicamente
            vector<int> iterIdx(n+1,0);
            vector<vector<int>> cached(n+1);
            auto getN = [&](int u)->vector<int>&{
                if (cached[u].empty()){
                    cached[u] = neighborsMat(u);
                }
                return cached[u];
            };
            while(!st.empty()){
                int u=st.back();
                auto &N = getN(u);
                if (iterIdx[u] < (int)N.size()){
                    int v = N[iterIdx[u]++];
                    if (level[v]==-1){
                        parent[v]=u; level[v]=level[u]+1;
                        st.push_back(v);
                    }
                } else {
                    st.pop_back();
                }
            }
        }
        return {parent, level};
    }

    // Distância por BFS simples entre u e v
    int dist(int s, int t) const {
        if (s<1 || s>n || t<1 || t>n) return -1;
        pair<vector<int>,vector<int>> pr = bfsTree(s);
        return pr.second[t];
    }

    // Componentes conexas
    vector<vector<int>> connectedComponents() const {
        vector<vector<int>> comps;
        vector<char> vis(n+1,0);
        deque<int> q;
        auto push_if = [&](int x){
            if (!vis[x]) { vis[x]=1; q.push_back(x); }
        };

        auto expand = [&](int u, vector<int>& comp){
            if (rep==LIST){
                for (int v: neighborsList(u)) push_if(v);
            } else {
                size_t base = (size_t)(u-1)*n;
                for (int v=1; v<=n; ++v){
                    if (mat[base + (size_t)(v-1)]) push_if(v);
                }
            }
            comp.push_back(u);
        };

        for (int s=1; s<=n; ++s){
            if (vis[s]) continue;
            q.clear(); vis[s]=1; q.push_back(s);
            vector<int> comp; comp.reserve(256);
            while(!q.empty()){
                int u=q.front(); q.pop_front();
                expand(u, comp);
            }
            sort(comp.begin(), comp.end());
            comps.push_back(move(comp));
        }
        // ordenar por tamanho desc
        sort(comps.begin(), comps.end(), [](const vector<int>& a, const vector<int>& b){
            return a.size() > b.size();
        });
        return comps;
    }

    // Eccentricidade a partir de s (retorna (ecc, argmax))
    pair<int,int> eccentricityFrom(int s) const {
        pair<vector<int>,vector<int>> pr = bfsTree(s);
        const vector<int> &lvl = pr.second;
        int ecc = -1, arg = s;
        for (int v=1; v<=n; ++v){
            if (lvl[v] > ecc){
                ecc = lvl[v]; arg = v;
            }
        }
        return make_pair(ecc, arg);
    }

    // Diâmetro aproximado (double sweep) com k repetições
    long long diameterApprox(int k, unsigned seed=7) const {
        mt19937 rng(seed);
        uniform_int_distribution<int> distu(1, n);
        long long best = -1;
        for (int i=0;i<k;++i){
            int s = distu(rng);
            pair<int,int> e1 = eccentricityFrom(s);
            pair<int,int> e2 = eccentricityFrom(e1.second);
            if (e2.first > best) best = e2.first;
        }
        return best;
    }

    // Escrita da árvore (BFS/DFS) em arquivo: "v parent[v] level[v]"
    void writeTree(const string& outPath, const vector<int>& parent, const vector<int>& level) const {
        ofstream out(outPath);
        if (!out) throw runtime_error("Não consegui escrever em: " + outPath);
        out << "# v parent[v] level[v]\n";
        for (int v=1; v<=n; ++v){
            out << v << " " << parent[v] << " " << level[v] << "\n";
        }
    }

    // Estatísticas e componentes (agora com maior_comp e menor_comp)
    void writeStats(const string& outPath) const {
        ofstream out(outPath);
        if (!out) throw runtime_error("Não consegui escrever em: " + outPath);

        // graus
        vector<int> deg(n+1,0);
        if (rep==LIST){
            for (int v=1; v<=n; ++v) deg[v] = (int)adj[v].size();
        } else {
            for (int v=1; v<=n; ++v){
                size_t base = (size_t)(v-1)*n;
                int d=0;
                for (int u=1; u<=n; ++u) if (mat[base+(size_t)(u-1)]) ++d;
                deg[v]=d;
            }
        }
        long long sum=0;
        int dmin=INT_MAX, dmax=0;
        vector<int> degs; degs.reserve(n);
        for (int v=1; v<=n; ++v){
            sum += deg[v];
            dmin = min(dmin, deg[v]);
            dmax = max(dmax, deg[v]);
            degs.push_back(deg[v]);
        }
        double media = (n? double(sum)/double(n) : 0.0);
        sort(degs.begin(), degs.end());
        double mediana = 0.0;
        if (n){
            if (n%2==1) mediana = degs[n/2];
            else mediana = (degs[n/2-1] + degs[n/2]) / 2.0;
        }

        out.setf(std::ios::fixed); out<<setprecision(3);
        out << "V " << n << "\n";
        out << "E " << m << "\n";
        out << "grau_min " << dmin << "\n";
        out << "grau_max " << dmax << "\n";
        out << "grau_medio " << media << "\n";
        out << "mediana_grau " << mediana << "\n";

        // componentes
        vector<vector<int>> comps = connectedComponents();
        out.unsetf(std::ios::floatfield);
        out << "componentes " << comps.size() << "\n";

        int maxsz = 0;
        int minsz = (int)(comps.empty()? 0 : comps.back().size());
        if (!comps.empty()){
            maxsz = (int)comps.front().size(); // comps[0] é a maior (ordenado desc)
            minsz = (int)comps.back().size();  // última é a menor
        }

        for (size_t i=0;i<comps.size();++i){
            const vector<int>& C = comps[i];
            out << "comp_" << (i+1) << "_tam " << C.size() << "\n";
            out << "comp_" << (i+1) << "_vertices";
            for (int v: C) out << " " << v;
            out << "\n";
        }
        // Linhas novas pedidas:
        out << "maior_comp " << maxsz << "\n";
        out << "menor_comp " << minsz << "\n";
    }

    // Memória estimada em MB
    double memoryMB() const {
        if (rep==LIST){
            // approx: 2*m ints
            double bytes = double(2.0*m) * sizeof(int);
            return bytes / (1024.0*1024.0);
        } else {
            double bytes = double(n) * double(n); // uint8_t
            return bytes / (1024.0*1024.0);
        }
    }
};
