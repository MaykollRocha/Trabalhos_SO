/***
    Disciplina: Sistemas Operacionais - 2023 - 1
    Faculdade: FACET, UFGD
    Docente: Marcos Paulo Moro
    Discente: Maykoll Rocha
    Os materiais utilizados como referência foram adicionados ao relatório,
    mas nos trechos onde estão, os links são providenciados.
***/


/**************************** BIBLIOTECAS ********************************/
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <conio.h>
#include <vector>
#include <process.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <thread> // Para ver o maximo de therdas
using namespace std;


/************************************ VariaVeis da Therds **********************************/
//Contrala a soma dos valores
HANDLE SecCrtSoma;

//Controla a retirada dos valores da threds
HANDLE SecCrtRetirada;

//Incia as variaies de Threads
int num_threads;


/************************************ Structs Gerais **********************************/

typedef struct {
    int l;
    int c;
}submatrizes;

typedef struct {
    bool existe;
    int Linha;
    int Coluna;
    int seed;
    long long** matriz;
}init;//Para a futura grande matriz

typedef struct {
    int lin;
    int col;
}infsubmatriz;//para informa o pedido do cliente para tetnar fazer as submatrizes

typedef struct {
    submatrizes** m;//O local que ela fica alocada
    int l;//Linha do matriz
    int c;//Coluna do submatriz
    int pross;//Checa se ele já foi lido
}matrizes;//Criar o vetor de submatrizes

typedef struct {
    int id;//Id da thred 
    int count;//Quantas matrizes ela processou
}PARAM;

/******************************************************* VARIAVEIS GLOBAIS ****************************************************************************/

//Vetor que huardas as submatrizes
vector<matrizes> vet_mat;

//Iniciando a matriz principal do codigo
init Matriz_mae;

//Armazenas informações para criar a SubMatriz 
infsubmatriz submatriz;

//variaves para somar
long int sum;

/******************************************************* Prototipação ****************************************************************************/

//Alocar a matriz muito muito grande e já encher tambem foi tirado da intenet com umas pequenas alterações
long long** AlocaMatriz(int l, int c, int seed);

//Checa se é primo tirado da internet com pequenas modificações
int chek_primo(long long numero);

//Checagem Sereal
int checagem_sereal(long long** m, int c, int l, int*tempo_s);

//Entra com a matriz e checa um por um
int funcao_somatoria_primos_paralelo(int Coluna, int Linha, submatrizes**aux);

/*
Separar em subMatrizes
Eu uso vários critérios de alocação 
Primeiro eiu faço o maximo possivel de submatrizes de mxn 
caso ainda sobre valor eu faço a sob matriz mais perto de uma 
matriz quadra m1xn1 que sera menor que a matris mxn mas caso esse
valor  sobresalente seja um primo não tem como fazer uma matriz m1,n1
logo eu faço um vetor primox1
*/

// Aloca os endereço da Matriz na SubMatriz em geral
submatrizes** AlocaSubMatriz(vector<int> colunas, vector<int> linhas, int kl, int ll);

//Aloca o restos dos enderaços que sobra no vetor de resto
submatrizes** AlocaRest(vector<submatrizes>*resto, int tl, int tc);

// Faz os calculos para subdividir a submatriz e apos isso já enche o vetor com as submatrizes
void sep_matriz(int l, int c, int tl, int tc);

// Função para fazer da thred
void my_parelelar_busca_prims(void* param);

//Retorna o tempo que processou e mostra quanto cada processador processou por motivos de interres
int processar_em_paralelo();

//Auxiliares

/*
    Um procedimento para checar as submatriz como elas se constituem
    ele mostra:
    quantas linhas e colunas foram armazenas
    quantos,quais valores tem por submatriz
    quantas sub matrizes
    todas essas informações são curiosas para fim de saber
    se tememos o menor e melhor numero de submatrizes que é
    caso MxN/(mxn) da um inteiro perfeito: MxN/(mxn)
    caso contrário: MxN/(mxn) +1 sendo esse 1 uma matriz menor que mxn ou um vetor caso seja um primo o resto
*/
void vet_info_matrizes();

//Usa para limpar o vetor de submatriz no final do processo
void limpar_vet_mat();
/******************************************************* MENU ****************************************************************************/


void menu()
{
    // uma matriz mãe muito muito pequena 
    int tempo_s;
    unsigned int max_n = std::thread::hardware_concurrency();
    long long total_prim;
    int tempo = 0;
    unsigned int op;
    Matriz_mae.existe = FALSE;
    Matriz_mae.seed = 2112;//Semente raiz pode ser torcada durante a execução
    bool ckeck_matrix_existence, valores_mudou_subvet, dados_suficiente_para_criar_matriz, sub_matriz_information, ja_rodamos_uma_vez, num_threads_info,valores_mudou;
    ckeck_matrix_existence = FALSE;//Checa se já existe matriz
    dados_suficiente_para_criar_matriz = FALSE;//Checa se você ja passou todos os parametros para matriz
    sub_matriz_information = FALSE;//Checa se você ja passou todos os parametros para submatriz
    ja_rodamos_uma_vez = FALSE;//Verifica se o codigo já foi rodado
    num_threads_info = FALSE;//Verifica se você informou as therds
    valores_mudou = FALSE;//Ve se mudou algum valor importante e não mudou a thread
    valores_mudou_subvet = TRUE;// Ve se os valores do subvet

    while (TRUE)
    {
        op = -1;

        cout << "--------------MENU--------------\n" <<
            " 1) Definir o tamanho da matriz.  Exemplo 10.000 X 10.000\n" <<
            " 2) Definir semente para o gerador de números aleatórios\n" <<
            " 3) Preencher a matriz com números aleatórios\n" <<
            " 4) Definir o tamanho das submatrizes\n" <<
            " 5) Definir o número de Threads\n" <<
            " 6) Executar\n" <<
            " 7) Visualizar o tempo de execução e quantidade de números primos;\n" <<
            " 8) Encerrar\n" << endl;
        if (Matriz_mae.existe)
        {
        cout << "----------------------Matriz Mãe------------------------------" << endl;
        cout << "Tamanho: " << Matriz_mae.Linha << "X" << Matriz_mae.Coluna << ".\n";
        cout << "Semente: " << Matriz_mae.seed << ".\n";
        cout << "--------------------------------------------------------------" << endl;
        }
        do {
            cin >> op;
        } while (op < 1 || op > 8);

        switch (op)
        {
        case 1:
            system("cls");
            if (Matriz_mae.existe) {
                free(Matriz_mae.matriz);
                Matriz_mae.existe = FALSE;
            }
            cout << "Entre com os valores da linha da matriz mãe: ";
            cin >> Matriz_mae.Linha;
            cout << "Entre com os valores da Colunas da Matriz mãr: ";
            cin >> Matriz_mae.Coluna;
            cout << "Novas Configurações Atribuidas\n";
            dados_suficiente_para_criar_matriz = TRUE;
            valores_mudou = TRUE;
            system("pause");
            system("cls");
            break;
        case 2:
            system("cls");
            if (Matriz_mae.existe) {
                free(Matriz_mae.matriz);
                Matriz_mae.existe = FALSE;
            }
            cout << "Entre com o valor da semente que deseja: ";
            cin >> Matriz_mae.seed;
            cout << "Novas Configurações Atribuidas\n";
            valores_mudou = TRUE;
            system("pause");
            system("cls");
            break;
        case 3:
            system("cls");
            if (dados_suficiente_para_criar_matriz && valores_mudou)
            {
                if (Matriz_mae.existe) {
                    free(Matriz_mae.matriz);
                    Matriz_mae.existe = FALSE;
                }
                Matriz_mae.matriz = AlocaMatriz(Matriz_mae.Linha, Matriz_mae.Coluna, Matriz_mae.seed);
                Matriz_mae.existe = TRUE;
                ckeck_matrix_existence = TRUE;
                valores_mudou = FALSE;
                valores_mudou_subvet = TRUE;
                total_prim = checagem_sereal(Matriz_mae.matriz, Matriz_mae.Linha, Matriz_mae.Coluna, &tempo_s);
                cout << "Matriz foi criada\n";
            }
            else {
                cout << "Não tem informações de Linha e Coluna para se criar uma matriz\n";
            }

            system("pause");
            system("cls");
            break;
        case 4:
            system("cls");
            if (dados_suficiente_para_criar_matriz)
            {
                do {
                    cout << "Entre com os valores da linha da SubMatriz: ";
                    cin >> submatriz.lin;

                    if (submatriz.lin < 0 || submatriz.lin > Matriz_mae.Linha)cout << "O numo informado é fora do permitido." << endl;
                } while (submatriz.lin < 0 || submatriz.lin > Matriz_mae.Linha);
                do {
                    cout << "Entre com os valores da Colunas da SubMatriz: ";
                    cin >> submatriz.col;

                    if (submatriz.col < 0 || submatriz.col > Matriz_mae.Coluna) cout << "O numo informado é fora do permitido." << endl;
                } while (submatriz.col < 0 || submatriz.col > Matriz_mae.Coluna);

                cout << "Novas Configurações Atribuidas Para Sub Matriz\n";
                sub_matriz_information = TRUE;
                valores_mudou_subvet = TRUE;
            }
            else {
                cout << "Não se pode infor uma submatriz sem saber os limites espressos quando se cria uma matriz";
            }

            system("pause");
            system("cls");
            break;
        case 5:
            system("cls");
            cout << "Defina o numero de threds: ";
            do {
                cin >> num_threads;
                if(num_threads <= 0 || num_threads >(int)(max_n * 4))
                {
                    cout << "Não pode ser esse valor\n Entre com outro(1~"<<max_n * 4<<")" << endl;;
                }
                if (num_threads > (int)(max_n * 4))
                {
                    char resp;
                    cout << "O numero selecionado é maior que o de therds que continuar? S/N : ";
                    cin >> resp;
                    if (resp == 'N' || resp == 'n')
                    {
                        num_threads = 0;
                    }
                }
            } while (num_threads <= 0 || num_threads > (int)(max_n * 4));
            cout << "Novas Configurações Atribuidas Para threds\n";
            num_threads_info = TRUE;
            system("pause");
            system("cls");
            break;
        case 6:
            system("cls");
            sum = 0;
            if (ckeck_matrix_existence && sub_matriz_information && num_threads_info && !valores_mudou)
            {
                cout << "Criando SubMatrizes" << endl;
                if (valores_mudou_subvet) {
                    if(vet_mat.size()) limpar_vet_mat();
                    sep_matriz(Matriz_mae.Linha, Matriz_mae.Coluna, submatriz.lin, submatriz.col);
                    valores_mudou_subvet = FALSE;
                }else if(ja_rodamos_uma_vez){
                    for (int i = 0; i < vet_mat.size(); i++)
                        vet_mat[i].pross = 0;
                }
                cout << "Processando as em Paralelo\n";
                tempo = processar_em_paralelo();
                cout << "Tudo certo aqui\n";
                ja_rodamos_uma_vez = TRUE;

            }
            else {
                if (!sub_matriz_information)cout << "Não temos a informação da submatrix\n";
                if (!ckeck_matrix_existence)cout << "Não temos a informação da matrix\n";
                if (!num_threads_info) cout << "Não foi informado o numero de threads\n";
                if(valores_mudou ) cout << "Valores murdaram mas nova Matriz ainda não criada\n";
            }
            system("pause");
            system("cls");
            break;
        case 7:
            system("cls");
            cout << "--DADOS DA MATRIZ--" << endl;
            if (num_threads_info) {
                cout << "Com " << num_threads << " threds.\n";
            }
            else {
                cout << "Não foi informado o numero de threads\n";
            }

            cout << "Na semente: " << Matriz_mae.seed << ".\n";
            if (ckeck_matrix_existence) {
                cout << "Com uma Matriz " << Matriz_mae.Linha << "X" << Matriz_mae.Coluna << ".\n";
            }
            else {
                cout << "Não temos a informação da matrix\n";
            }
            if (sub_matriz_information)
            {
                cout << "A submatrizes são formadas por " << submatriz.lin << "X" << submatriz.col << endl;
            }
            else {
                cout << "Não Temos a Submatriz Informada" << endl;
            }
            if (ja_rodamos_uma_vez && !valores_mudou)
            {
                cout << "--RODADO SEREALMENTE--" << endl;
                cout << "O tempo para busca sereal foi de: " << ((double)tempo_s) / CLOCKS_PER_SEC << "s.\n";
                cout << "Seralemente é para dar: " << total_prim << endl;
                cout << "--RODADO PARALELAMENTE--" << endl;
                cout << "Demorou um tempo de " << ((double)tempo) / CLOCKS_PER_SEC << "s.\n";
                cout << "Foram contabilizadas " << sum << "primos.\n";
            }
            else {
                cout << "Codigo ainda não fez sua função matricional\n";
            }

            system("pause");
            system("cls");
            break;
        case 8: 
            system("cls");
            return;

        default:
            system("cls");
            
        }
    }



}

int main()
{
    setlocale(LC_ALL, "Portuguese");

    menu();


    //Limpa o vetor que aloca as submatrizes
    if (vet_mat.size()) limpar_vet_mat();
    //Limpa o vetor que aloca a grande matriz
    if (Matriz_mae.existe) {
        free(Matriz_mae.matriz);
        Matriz_mae.existe = FALSE;
    }
    return 1;
}

/******************************************************* FUÇÕES Básica ****************************************************************************/

/*
Oque tem aqui:
Alocar matriz
Encher matriz
Checar primos
Calcular Primos
*/

long long** AlocaMatriz(int l, int c, int seed) {
    long long** M;
    int i;

    M = (long long**)malloc(sizeof(long long*) * l);
    if (M == NULL) {
        exit(1);
    }
    for (i = 0; i < l; i++) {
        M[i] = (long long*)malloc(sizeof(long long) * c);
        if (M[i] == NULL) {
            exit(1);
        }
    }

    srand(seed);
    for (int i = 0; i < c; i++)
        for (int k = 0; k < l; k++)
            M[k][i] = rand() % 99999999;

    return M;
}

int chek_primo(long long numero)
{
    if (numero <= 1)
        return 0;

    if (numero <= 3)
        return 1;

    if (numero % 2 == 0 || numero % 3 == 0)
        return 0;

    int i = 5;
    while (i * i <= numero)
    {
        if (numero % i == 0 || numero % (i + 2) == 0)return 0;
        i += 6;
    }

    return 1;
}

int checagem_sereal(long long** m, int l, int c,int *tempo_s)
{
    int cas;
    cas = 0;
    int init, find;
    init = clock();
    for (int j = 0; j < c; j++)
        for (int k = 0; k < l; k++)
            cas += chek_primo(m[k][j]);
    find = clock();
    *tempo_s = (find - init);
    return cas;
}

int funcao_somatoria_primos_paralelo(int Coluna, int Linha,submatrizes**aux)
{
    int cont;
    cont = 0;  
    for (int i = 0; i < Coluna; i++)
        for (int k = 0; k < Linha; k++)
            cont += chek_primo(Matriz_mae.matriz[aux[k][i].l][aux[k][i].c]);
    return cont;
}

/******************************************************* Funções de Submatriz ****************************************************************************/

submatrizes** AlocaSubMatriz(vector<int> colunas, vector<int> linhas, int kl, int ll) {
    submatrizes** M;
    int i;

    M = (submatrizes**)malloc(sizeof(submatrizes*) * (linhas[ll] - linhas[ll - 1] + 1));
    if (M == NULL) {
        exit(1);
    }
    for (i = 0; i < (linhas[ll] - linhas[ll - 1]); i++) {
        M[i] = (submatrizes*)malloc(sizeof(submatrizes) * (colunas[kl] - colunas[kl - 1] + 1));
        if (M[i] == NULL) {
            exit(1);
        }
    }

    for (int k1 = 0, k = colunas[kl - 1]; k < colunas[kl]; k1++, k++)
    {

        for (int j1 = 0, j = linhas[ll - 1]; j < linhas[ll]; j1++, j++)
        {
            M[j1][k1].l = j;
            M[j1][k1].c = k;
        }
    }

    return M;
}
submatrizes** AlocaRest(vector<submatrizes>* resto, int tl,int tc)
{
    submatrizes** M;
    int i;
    M = (submatrizes**)malloc(sizeof(submatrizes*) * (tl));
    if (M == NULL) {
        exit(1);
    }
    for (i = 0; i < (tl); i++) {
        M[i] = (submatrizes*)malloc(sizeof(submatrizes) * (tc));
        if (M[i] == NULL) {
            exit(1);
        }
    }
    for (int k = 0; k < tc; k++)
    {
        for (int j = 0; j < tl; j++)
        {
            submatrizes trash;
            trash = resto->back();
            resto->pop_back();
            M[j][k].l = trash.l;
            M[j][k].c = trash.c;
        }
    }

    return M;
}

void sep_matriz(int l, int c, int tl, int tc)
{
    int l0, c0;//Valor incial onde minhas matriz estra
    int nmc = (c - (c % tc)) / tc;//Passos na coluna da minha matriz
    int nml = (l - (l % tl)) / tl;//Passos na linha da minha matriz
    int rc = c % tc;//Resto que sobra quando a matriz não da colunas perfeita
    int rl = l % tl;//Resto que sobra quando a matriz não da linhas perfeitas
    // tudo inicia do [0,0]
    //Setando parametros inicias
    c0 = 0;
    l0 = 0;
    int count;
    count = 0;
    vector<int> linhas;
    vector<int> colunas;
    vector<submatrizes> resto;//Esse funcina especialmente para 
    linhas.push_back(l0);
    colunas.push_back(l0);

    //Criando stacs de valores para fazer a submatriz MxN
    while (nml > count)
    {
        int l1 = l0 + tl;
        linhas.push_back(l1);

        l0 = l1;

        count++;
    }
    count = 0;
   
    while (nmc > count)
    {
        int c1 = c0 + tc;
        colunas.push_back(c1);
        c0 = c1;
        count++;

    }
    // Armazenando resto
    if (rc) {
        colunas.push_back(colunas[colunas.size() - 1] + rc);
        nmc++;
    }
    if (rl) {
        linhas.push_back(linhas[linhas.size() - 1] + rl);
        nml++;
    }

    //Aquile ele trabalha colunas por linhas
    for (int kl = 1; kl < nmc + 1; kl++)
    {
        //Trabalho por linhas
        for (int ll = 1; ll < nml + 1; ll++)
        {
            //Cria uma auxiliar 
            matrizes aux;
            aux.l = linhas[ll] - linhas[ll - 1];
            aux.c = colunas[kl] - colunas[kl - 1];
            aux.pross = 0;
            // caso consiga fazer uma submatriz com o tamanho correto ele faz caso não ele guarda os valores para fazer uso no futuro
            if (aux.l * aux.c == tl * tc) {
                aux.m = AlocaSubMatriz( colunas, linhas, kl, ll);
                vet_mat.push_back(aux);
            }
            else {
                for (int k1 = 0, k = colunas[kl - 1]; k < colunas[kl]; k1++, k++){
                    for (int j1 = 0, j = linhas[ll - 1]; j < linhas[ll]; j1++, j++){
                        submatrizes aux2;
                        aux2.l = j;
                        aux2.c = k;
                        resto.push_back(aux2);
                    }
                }
            }
            
            
        }
    }
    //Limpa as linhas e colunas que não é mais nescessário já que todos os enderaços já foram pré setados
    linhas.clear();
    colunas.clear();

    // caso não prescise fazer mais submatrizes ele para de trabalhar
    if (resto.size() == 0) return;

    //faz o numero maximo de submatriz MxN
    while (resto.size() >= (tc*tl)){
        matrizes aux;
        aux.l = tl;
        aux.c = tc;
        aux.pross = 0;
        aux.m = AlocaRest(&resto,tl,tc);
        vet_mat.push_back(aux);
    } 

    //Se restar algo ainda ele faz uma submatriz menor mxn que vai ser a unica diferente de todas as outras
    if (resto.size())
    {
        int rcs;
        rcs = 1;
        //ele não consegue contruir um matriz com valor primo, não exite matriz de valor de numero primo apenas vetor.
        if (chek_primo(resto.size())!=1) {
            int i = 2;
            //  Aqui ele procura os dois mmenores numero para 
            //  Alcançar a solução
            //  EX: se resta 20 valores para bor em uma sumatriz
            //  Ele pode fazer de 2 geitos 2x10 ou 4x5 matematicamente falando
            //  Melhor caso é o 4x5 que seria uma matriz mais redoda isso que ele faz
            //  Se for 100 nos podemos faze 1x100, 2x50,4x25,5x20,10x10, matematicamente a melhor
            //  Sera a 10 po 10 que somado é 20
            //  Tinha duas maneiras de acaçar esse valor uma seria pegando todo e vendo a menor soma
            //  101,52,29,25,20 ou você segue a ideia de Busca binaria
            //      1. Começo minha busca de 2 a Numeroa de valores que resta
            //      2. busco o proxo %i == 0 se econtra esse n é o novo valor de linha
            //      3. com isso incremento i
            //      4. busca é feita até que i < resto/i
            // com isso acho a menor soma automaticamente
            while (i < resto.size() / rcs) {
                if (resto.size() % i==0)rcs = i;
                i++;
            }    
        }
        int rls = (int)(resto.size())/ rcs;

        matrizes aux;
        aux.l = rls;
        aux.c = rcs;
        aux.pross = 0;
        aux.m = AlocaRest(&resto, rls, rcs);
        vet_mat.push_back(aux);
    }
    resto.clear();
}

/******************************************************* Funções de Processamento ****************************************************************************/

void my_parelelar_busca_prims(void* param)
{
    // Reber os parametros externos
    PARAM* paramt = (PARAM*)param;
    //Variaveis locais das threds
    bool sair_do_laco;//para ver se toda matriz foi percorrida
    sair_do_laco = FALSE;
    bool tem_id;//Para ver se tem algo para ver averiguado
    tem_id = FALSE;
    matrizes aux;//Para auxiliar pegar a submatriz que contem os indeces
    int somas;//Auxiliar temporaria que pega a soma da submatriz
    somas = 0;
    //Verifica se há submatris suficiente para cada indice
    if (vet_mat.size() > paramt->id)
    {
        aux = vet_mat[paramt->id];
        vet_mat[paramt->id].pross = 1;
        tem_id = TRUE;
    }

    while (TRUE)
    {   
        //Checa se ele conseguiu pegar algo logo de inicio
        if (tem_id) {
            somas = funcao_somatoria_primos_paralelo(aux.c, aux.l,aux.m);// Faz a soma uma a um e já coloca no epaço da soma
            paramt->count += 1;//Conta o numero de matrizes analisadas
        }
        
        //Faz uma seção critica para soma
        WaitForSingleObject(SecCrtSoma, INFINITE);//Sessão para espera dos outros processo
        sum += somas;
        ReleaseMutex(SecCrtSoma);
        //Faz uma sessão crítcia para tirar valores da sub matriz
        WaitForSingleObject(SecCrtRetirada, INFINITE);//Sessão para espera dos outros processo
        if (num_threads > vet_mat.size()) sair_do_laco = TRUE;
        
        //Comça no numero de treads
        for (int i = num_threads; i < vet_mat.size(); i++)
        {
            if (vet_mat[i].pross == 0)//Checa se já não foi consumida
            {
                vet_mat[i].pross = 1;
                aux = vet_mat[i];
                break;
            }
            //Confirma que já foi tudo averiguado e sai da matriz
            if ((i + 1) == vet_mat.size()) {
                sair_do_laco = TRUE;
                break;
            }
        }
        ReleaseMutex(SecCrtRetirada);
        // Caso todos os valores da sub matriz tenha sido checado e todos apresentão como processado sai da sub matriz
        if (sair_do_laco) break;
    }

    // Termina Thread
    _endthread();
}

int processar_em_paralelo()
{   
    vector<HANDLE> hThread;// Armazenas as therds
    vector<PARAM> vParam;// Armazena os parametros de cada thread
    PARAM addve;// auxiliar para receber e enxer os parametros

    //Sessão critica de soma
    SecCrtSoma = CreateMutex(NULL, FALSE, NULL);
    // sessão critoca de checar e pegar as submtrizes
    SecCrtRetirada = CreateMutex(NULL, FALSE, NULL);

    // Encher os parametros com o indice da thread e tbem para contar quantas threads ele pegou
    for (int i = 0; i < num_threads; i++)
    {
        addve.id = i;
        addve.count = 0;
        vParam.push_back(addve);
    }

    //Criar as threads todas suspensas
    for (int i = 0; i < num_threads; i++)
        hThread.push_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&my_parelelar_busca_prims, &vParam[i], CREATE_SUSPENDED, NULL));
    
    int init, find;// variaveis de conta clock
    
    //Informa o inicio do processo
    cout << "Começou os processos" << endl;

    //incia a contagem de tempo
    init = clock();

    // Incia as theads
    for (int i = 0; i < num_threads; i++) {
        ResumeThread(hThread[i]);
    }

    // Espera todos os objetos terminar
    WaitForMultipleObjects(num_threads, hThread.data(), TRUE, INFINITE);

    //Fecha as treads
    for (int i = 0; i < num_threads; i++) {
        CloseHandle(hThread[i]);
    }

    //Para de contar o tempo
    find = clock(); 

    //Informa que acabou o processo
    cout << "saiu os processos" << endl;

    //Fecha as sessões crítica
    if (SecCrtSoma != 0)
        CloseHandle(SecCrtSoma);
    if (SecCrtRetirada != 0)
        CloseHandle(SecCrtRetirada);
    
    //Conta o numero de submatriz processada
    int sun_matrix_process = 0;
    cout << "Informações apos o termino do processamento:" << endl;

    //Informa qunato cada indice processou
    for (int i = 0; i < num_threads; i++) {
        cout << "Processador de identidade [" << vParam[i].id << "]" << "processou: " << vParam[i].count << endl;
        sun_matrix_process += vParam[i].count;
    }

    //Mostra quantas foram processadas
    cout << sun_matrix_process << endl;

    //Retorna o clok to tempo
    return find - init;

}

/******************************************************* Funções de Auxiliares ****************************************************************************/

void vet_info_matrizes()
{
    /*
    Um procedimento para checar as submatriz como elas se constituem
    ele mostra:
    quantas linhas e colunas foram armazenas
    quantos,quais valores tem por submatriz
    quantas sub matrizes
    todas essas informações são curiosas para fim de saber
    se tememos o menor e melhor numero de submatrizes que é
    caso MxN/(mxn) da um inteiro perfeito: MxN/(mxn)
    caso contrário: MxN/(mxn) +1 sendo esse 1 uma matriz menor que mxn ou um vetor caso seja um primo o resto
    */
    int count;
    count = 0;
    for (int i = 0; i < vet_mat.size(); i++)
    {
        cout << vet_mat[i].l << "X" << vet_mat[i].c << endl;
        for (int k = 0; k < vet_mat[i].c; k++)
        {
            for (int j = 0; j < vet_mat[i].l; j++)
            {
                cout << vet_mat[i].m[j][k].l <<"X"<< vet_mat[i].m[j][k].c<< " ";
                count++;
            }
            cout << endl;
        }
    }
    cout << "Numero de matrizes: " << vet_mat.size() << endl;
    cout << "Numeros na matriz: " << count << endl;
}

void limpar_vet_mat()
{
    //Usa para limpar o vetor de submatriz no final do processo
    while (vet_mat.size())
    {
        //Limpa o espaço de memoria dela
        free(vet_mat[vet_mat.size() - 1].m);
        //tira ela do vertor;
        vet_mat.pop_back();
    }

    return;
}

