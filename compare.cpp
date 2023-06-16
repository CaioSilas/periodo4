#include <iostream>
#include<string>

using namespace std;

int main(){
    string nome;
    nome = "caio";
    string nome1;
    nome1 ="caio";
    string nome2;
    nome2 ="carlos";

    if (nome.compare(nome1) == 0){
        cout << "os nomes sao iguais\n" << endl;
    }
    else {
        cout << "os nomes sao diferentes\n" << endl;
    }

    if (nome.compare(nome2) == 0){
        cout << "os nomes sao iguais\n" << endl;
    }
    else {
        cout << "os nomes sao diferentes\n" << endl;
    }



    return 0;
}