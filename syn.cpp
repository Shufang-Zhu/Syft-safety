#include "syn.h"

syn::syn(string filename, string partfile)
{
    //ctor

    //Cudd *p = &mgr;
    bdd.initialize(filename, partfile);
    mgr = *(bdd.mgr);
    initializer();

}

syn::~syn()
{
    //dtor
}

void syn::initializer(){
    for(int i = 0; i < bdd.nbits; i++){
        BDD b = mgr.bddVar();
        bdd.bddvars.push_back(b);
    }
    BDD tmp = mgr.bddZero();
    for(int i = 0; i < bdd.finalstates.size(); i++){
        BDD ac = state2bdd(bdd.finalstates[i]);
        tmp += ac;
    }
    W.push_back(!tmp);//modifty negation
    //dumpdot(I, "W00");
    Wprime.push_back(!tmp);//modify negation
    cur = 0;

}

BDD syn::state2bdd(int s){
    string bin = state2bin(s);
    BDD b = mgr.bddOne();
    int nzero = bdd.nbits - bin.length();
    //cout<<nzero<<endl;
    for(int i = 0; i < nzero; i++){
        b *= !bdd.bddvars[i];
    }
    for(int i = 0; i < bin.length(); i++){
        if(bin[i] == '0')
            b *= !bdd.bddvars[i+nzero];
        else
            b *= bdd.bddvars[i+nzero];
    }
    return b;

}

string syn::state2bin(int n){
    string res;
    while (n)
    {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }

    if (res.empty())
        res = "0";
    else
        reverse(res.begin(), res.end());
   return res;
}

bool syn::fixpoint(){
    if(W[cur] == W[cur-1])
        return true;
}

bool syn::realizablity(){
    while(true){

        BDD tmp = W[cur] * univsyn();//modify
        W.push_back(tmp);
        cur++;
        if(fixpoint())
            break;
        Wprime.push_back(existsyn());
        //assert(cur = (W.size() - 1));
    }
    //dumpdot(I, "W00");
    cout<<bdd.init<<endl;
    if((Wprime[cur-1].Eval(state2bit(bdd.init))).IsOne()){
        BDD O = mgr.bddOne();
        vector<BDD> S2O;
        for(int i = 0; i < bdd.output.size(); i++){
            //cout<<bdd.output[i]<<endl;
            O *= bdd.bddvars[bdd.output[i]];
        }
        //cout<<bdd.nbits<<endl;
        //"ALIVE"
        O *= bdd.bddvars[bdd.nbits];
        /*//naive synthesis
        W[cur].SolveEqn(O, S2O, outindex(), bdd.output.size());
        strategy(S2O);
        */
        InputFirstSynthesis IFsyn(mgr);
        unordered_map<unsigned int, BDD> IFstrategy = IFsyn.synthesize(W[cur], O);
        //IFstrategy = input_first(W[cur], O);
        return true;
    }

    return false;

}


void syn::strategy(vector<BDD>& S2O){
    vector<BDD> winning;
    for(int i = 0; i < S2O.size(); i++){
        for(int j = 0; j < bdd.output.size(); j++){
            int index = bdd.output[j];
            S2O[i] = S2O[i].Compose(bdd.bddvars[index], mgr.bddOne());
        }
    }
}

int** syn::outindex(){
    int outlength = bdd.output.size();
    int outwidth = 2;
    int **out = 0;
    out = new int*[outlength];
    for(int l = 0; l < outlength; l++){
        out[l] = new int[outwidth];
        out[l][0] = l;
        out[l][1] = bdd.output[l];
    }
    return out;
}

int* syn::state2bit(int n){
    string res;
    int s[bdd.nbits+1];
    while (n)
    {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }

    if (res.empty())
        res = "0";
    else
        reverse(res.begin(), res.end());
    //cout<<res<<endl;
    int offset = bdd.nbits - res.length();
    for(int i = 0; i < offset; i++)
        s[i] = 0;
    for(int i = offset; i < res.length(); i++)
        s[i] = (int(res[i]) - 48);
    s[bdd.nbits] = 1;
    return s;
}


BDD syn::univsyn(){
    BDD I = mgr.bddOne();
    BDD tmp = Wprime[cur];
    int index;
    int offset = bdd.nbits + bdd.nvars;
    for(int i = 0; i < bdd.input.size(); i++){
        index = bdd.input[i];
        I *= bdd.bddvars[index];
    }
    //dumpdot(I, "W00");
    tmp = prime(tmp);
    for(int i = 0; i < bdd.nbits; i++){
        tmp = tmp.Compose(bdd.res[i], offset+i);
    }

    //tmp *= !Wprime[cur];//modify delete this constraint

    BDD eliminput = tmp.UnivAbstract(I);
    return eliminput;

}

BDD syn::prime(BDD orign){
    int offset = bdd.nbits + bdd.nvars;
    BDD tmp = orign;
    for(int i = 0; i < bdd.nbits; i++){
        tmp = tmp.Compose(bdd.bddvars[i+offset], i);
    }
    return tmp;
}

BDD syn::existsyn(){
    BDD O = mgr.bddOne();
    BDD tmp = W[cur];
    int index;
    int offset = bdd.nbits + bdd.nvars;
    for(int i = 0; i < bdd.output.size(); i++){
        index = bdd.output[i];
        O *= bdd.bddvars[index];
    }
    BDD elimoutput = tmp.ExistAbstract(O);
    return elimoutput;

}

void syn::dumpdot(BDD &b, string filename){
    FILE *fp = fopen(filename.c_str(), "w");
    vector<BDD> single(1);
    single[0] = b;
	this->mgr.DumpDot(single, NULL, NULL, fp);
	fclose(fp);
}


















