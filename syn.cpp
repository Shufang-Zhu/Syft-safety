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
    tmp = greatest_acc(tmp);
    W.push_back(!tmp);//modifty negation
    //dumpdot(W[0], "w0");
    Wprime.push_back(!tmp);//modify negation
    cur = 0;
    // dumpdot(bdd.res[0], "s0");
    // dumpdot(bdd.res[1], "s1");

}

BDD syn::greatest_acc(BDD f){
    vector<BDD> acc;
    cur = 0;
    acc.push_back(f);
    BDD newf;
    while(true){
        newf = acc[cur] * elim_acc(acc[cur]);
        if(newf == acc[cur])
            break;
        acc.push_back(newf);
        cur++;
    }
    return newf;
}

BDD syn::elim_acc(BDD f){
    BDD V = mgr.bddOne();
    int index;
    for(int i = 0; i < bdd.input.size(); i++){
        index = bdd.input[i];
        V *= bdd.bddvars[index];
    }
    for(int i = 0; i < bdd.output.size(); i++){
        index = bdd.output[i];
        V *= bdd.bddvars[index];
    }
    f = prime(f);
    int offset = bdd.nbits + bdd.nvars;
    for(int i = 0; i < bdd.nbits; i++){
        f = f.Compose(bdd.res[i], offset+i);
    }
    BDD elimacc = f.UnivAbstract(V);
    return elimacc;

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
        BDD I = mgr.bddOne();
        int index;
        for(int i = 0; i < bdd.input.size(); i++){
            index = bdd.input[i];
            I *= bdd.bddvars[index];
        }
        BDD tmp = W[cur] * univsyn(I);//modify
        W.push_back(tmp);
		
        cur++;
		
        if(fixpoint())
            break;
        BDD O = mgr.bddOne();
        for(int i = 0; i < bdd.output.size(); i++){
            index = bdd.output[i];
            O *= bdd.bddvars[index];
        }
        Wprime.push_back(existsyn(O));
		// dumpdot(Wprime[cur], "w"+to_string(cur));
        if(!(Wprime[cur].Eval(state2bit(bdd.init))).IsOne()){
            return false;
        }
    }
    if((Wprime[cur-1].Eval(state2bit(bdd.init))).IsOne()){
        BDD O = mgr.bddOne();
        vector<BDD> S2O;
        for(int i = 0; i < bdd.output.size(); i++){
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


bool syn::realizablity_variant(){
    BDD transducer;
    while(true){
        int index;
        BDD O = mgr.bddOne();
        for(int i = 0; i < bdd.output.size(); i++){
            index = bdd.output[i];
            O *= bdd.bddvars[index];
        }

        BDD tmp = W[cur] * existsyn_invariant(O, transducer);
        W.push_back(tmp);
        cur++;
        if(fixpoint())
            break;
        BDD I = mgr.bddOne();
        for(int i = 0; i < bdd.input.size(); i++){
            index = bdd.input[i];
            I *= bdd.bddvars[index];
        }
        Wprime.push_back(univsyn_invariant(I));
        if(!(Wprime[cur].Eval(state2bit(bdd.init))).IsOne()){
            return false;
        }

    }
    if((Wprime[cur-1].Eval(state2bit(bdd.init))).IsOne()){
        BDD O = mgr.bddOne();
        vector<BDD> S2O;
        for(int i = 0; i < bdd.output.size(); i++){
            O *= bdd.bddvars[bdd.output[i]];
        }
        O *= bdd.bddvars[bdd.nbits];
        //naive synthesis
        transducer.SolveEqn(O, S2O, outindex(), bdd.output.size());
        strategy(S2O);

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
    // string res;
    // int s[bdd.nbits+1];
    // while (n)
    // {
    //     res.push_back((n & 1) + '0');
    //     n >>= 1;
    // }

    // if (res.empty())
    //     res = "0";
    // else
    //     reverse(res.begin(), res.end());

    // int offset = bdd.nbits - res.length();
    // for(int i = 0; i < offset; i++)
    //     s[i] = 0;
    // for(int i = offset; i < res.length(); i++)
    //     s[i] = (int(res[i]) - 48);
    // s[bdd.nbits] = 1;
    // return s;

    int* initbv = new int[bdd.nbits];
    int temp = n;
    for (int i = bdd.nbits - 1; i >= 0; i--) {
        initbv[i] = temp % 2;
        temp = temp / 2;
    }
    return initbv;
}


BDD syn::univsyn(BDD univ){

    BDD tmp = Wprime[cur];
    int offset = bdd.nbits + bdd.nvars;

    //dumpdot(I, "W00");
    tmp = prime(tmp);
    for(int i = 0; i < bdd.nbits; i++){
        tmp = tmp.Compose(bdd.res[i], offset+i);
    }

    //tmp *= !Wprime[cur];//modify delete this constraint

    BDD eliminput = tmp.UnivAbstract(univ);
    return eliminput;

}

BDD syn::univsyn_invariant(BDD univ){

    BDD tmp = W[cur];
    BDD elimuniv = tmp.UnivAbstract(univ);
    return elimuniv;

}

BDD syn::prime(BDD orign){
    int offset = bdd.nbits + bdd.nvars;
    BDD tmp = orign;
    for(int i = 0; i < bdd.nbits; i++){
        tmp = tmp.Compose(bdd.bddvars[i+offset], i);
    }
    return tmp;
}

BDD syn::existsyn_invariant(BDD exist, BDD& transducer){
    BDD tmp = Wprime[cur];
    int offset = bdd.nbits + bdd.nvars;

    //dumpdot(I, "W00");
    tmp = prime(tmp);
    for(int i = 0; i < bdd.nbits; i++){
        tmp = tmp.Compose(bdd.res[i], offset+i);
    }
    transducer = tmp;
    BDD elimoutput = tmp.ExistAbstract(exist);
    return elimoutput;

}

BDD syn::existsyn(BDD exist){

    BDD tmp = W[cur];
    BDD elimoutput = tmp.ExistAbstract(exist);
    return elimoutput;

}

void syn::dumpdot(BDD &b, string filename){
    FILE *fp = fopen(filename.c_str(), "w");
    vector<BDD> single(1);
    single[0] = b;
	this->mgr.DumpDot(single, NULL, NULL, fp);
	fclose(fp);
}


















