#include <bits/stdc++.h>
#include "graph.hpp"
using namespace std;

static void usage() {
    cerr <<
    "Uso:\n"
    "  --in=arquivo.txt --rep=list|matrix\n"
    "  --stats=saida.txt\n"
    "  --bfs=S --out=arq.txt\n"
    "  --dfs=S --out=arq.txt\n"
    "  --dist=u,v\n"
    "  --diameter\n"
    "  --diameter-approx=k\n"
    "  --bench-bfs=R\n"
    "  --bench-dfs=R\n";
}

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string inFile, repStr;
    string statsOut;
    string outFile;
    int bfsRoot = -1, dfsRoot = -1;
    pair<int,int> distPair = make_pair(-1,-1);
    bool wantDiameterExact = false;
    int diameterK = -1;
    int benchBfs = 0, benchDfs = 0;

    auto getVal = [&](const string& key)->string{
        for (int i=1;i<argc;++i){
            string a = argv[i];
            if (a.rfind(key,0)==0) return a.substr(key.size());
        }
        return "";
    };

    // parse
    for (int i=1;i<argc;++i){
        string a = argv[i];
        if (a.rfind("--in=",0)==0) inFile = a.substr(5);
        else if (a.rfind("--rep=",0)==0) repStr = a.substr(6);
        else if (a.rfind("--stats=",0)==0) statsOut = a.substr(8);
        else if (a.rfind("--bfs=",0)==0) bfsRoot = stoi(a.substr(6));
        else if (a.rfind("--dfs=",0)==0) dfsRoot = stoi(a.substr(6));
        else if (a.rfind("--out=",0)==0) outFile = a.substr(6);
        else if (a.rfind("--dist=",0)==0) {
            string v = a.substr(7);
            size_t p = v.find(',');
            if (p!=string::npos){
                distPair.first = stoi(v.substr(0,p));
                distPair.second = stoi(v.substr(p+1));
            }
        }
        else if (a=="--diameter") wantDiameterExact = true;
        else if (a.rfind("--diameter-approx=",0)==0) diameterK = stoi(a.substr(18));
        else if (a.rfind("--bench-bfs=",0)==0) benchBfs = stoi(a.substr(12));
        else if (a.rfind("--bench-dfs=",0)==0) benchDfs = stoi(a.substr(12));
        else {
            cerr << "Arg desconhecido: " << a << "\n";
            usage();
            return 1;
        }
    }

    if (inFile.empty() || repStr.empty()){
        usage(); return 1;
    }

    Graph::Rep rep;
    if (repStr=="list") rep = Graph::LIST;
    else if (repStr=="matrix") rep = Graph::MATRIX;
    else {
        cerr << "rep inválido: " << repStr << "\n";
        return 1;
    }

    Graph G = Graph::loadFromFile(inFile, rep);
    cerr.setf(std::ios::fixed); cerr<<setprecision(3);
    cerr << "Carregado: V=" << G.n
         << " E=" << G.m
         << " rep=" << (rep==Graph::LIST? "LIST":"MATRIX")
         << " mem_estimada_MB=" << G.memoryMB()
         << "\n";
    cerr.unsetf(std::ios::floatfield);

    // --stats
    if (!statsOut.empty()){
        G.writeStats(statsOut);
        // continua para permitir combinar flags
    }

    // --bfs / --dfs (precisam de --out)
    if (bfsRoot!=-1){
        if (outFile.empty()){
            cerr << "Para --bfs=S informe também --out=arquivo\n";
            return 1;
        }
        pair<vector<int>,vector<int>> pr = G.bfsTree(bfsRoot);
        G.writeTree(outFile, pr.first, pr.second);
    }
    if (dfsRoot!=-1){
        if (outFile.empty()){
            cerr << "Para --dfs=S informe também --out=arquivo\n";
            return 1;
        }
        pair<vector<int>,vector<int>> pr = G.dfsTree(dfsRoot);
        G.writeTree(outFile, pr.first, pr.second);
    }

    // --dist=u,v
    if (distPair.first!=-1){
        int d = G.dist(distPair.first, distPair.second);
        cout << "dist(" << distPair.first << "," << distPair.second << ")=" << d << "\n";
    }

    // --diameter (exato): em grafos grandes, recusar e sugerir approx
    if (wantDiameterExact){
        if (G.n > 4000){
            cerr << "Grafo grande: use --diameter-approx — tente --diameter-approx=k\n";
        } else {
            long long best = -1;
            for (int s=1; s<=G.n; ++s){
                pair<vector<int>,vector<int>> pr = G.bfsTree(s);
                const vector<int> &lvl = pr.second;
                for (int v=1; v<=G.n; ++v) if (lvl[v]>best) best=lvl[v];
            }
            cout << "diameter=" << best << "\n";
        }
    }

    // --diameter-approx=k
    if (diameterK>0){
        long long d = G.diameterApprox(diameterK);
        cout << "diameter_approx=" << d << "\n";
    }

    // Benchmarks (tempo médio por execução, em ms)
    auto bench = [&](bool doBfs, int reps){
        using clk = chrono::steady_clock;
        vector<int> roots = {1};
        if (G.n >= 2) roots.push_back(G.n/2);
        if (G.n >= 3) roots.push_back(G.n);
        auto t0 = clk::now();
        int rpos = 0;
        for (int i=0;i<reps;++i){
            int s = roots[rpos];
            rpos = (rpos+1) % (int)roots.size();
            if (doBfs) (void)G.bfsTree(s);
            else       (void)G.dfsTree(s);
        }
        auto t1 = clk::now();
        double ms = chrono::duration<double, milli>(t1-t0).count() / max(1,reps);
        return ms;
    };

    if (benchBfs>0){
        double ms = bench(true, benchBfs);
        cout.setf(std::ios::fixed); cout<<setprecision(3);
        cout << "bench_bfs_ms=" << ms << "\n";
        cout.unsetf(std::ios::floatfield);
    }
    if (benchDfs>0){
        double ms = bench(false, benchDfs);
        cout.setf(std::ios::fixed); cout<<setprecision(3);
        cout << "bench_dfs_ms=" << ms << "\n";
        cout.unsetf(std::ios::floatfield);
    }

    return 0;
}
