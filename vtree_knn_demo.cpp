#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>
#include<ctime>
#include<fstream>
#include<algorithm>
#include<map>
#include<cmath>
#include<queue>
#include<sys/time.h>
#include<metis.h> // Conjunto de programas en serie para particionar gráficos, particionar mallas de elementos finitos y producir órdenes de reducción de relleno para matrices dispersas.

using namespace std;
const bool DEBUG_=false;
const char *Edge_File;
const int Partition_Part=4; // Fanout del vtree
const int Naive_Split_Limit=33; // Numero de nodos hoja
bool Save_Order_Matrix=false; // // Si necesita soporte para la ruta Matrix(Si es true puede tener un uso adicional)
const int INF=0x3fffffff;
const bool RevE=false; //Si es necesario hacer alguna copia, grafo dirigido:false，grafo indirecto:true
const bool Distance_Offset=false; // Si se considera el desplazamiento del vehículo.
const bool DEBUG1=false;
bool NWFIX ;

const bool Optimization_KNN_Cut=true; // Optimización para KNN 
const bool Memory_Optimization=true;  // Optimización de la memoria (normalmente activada)
const bool INDEX_Optimization=true;   // Optimización del Índice, si necesita almacenar más información desactívelo (normalmente no es necesario)

struct timeval tv;
long long ts, te;
void TIME_TICK_START() {gettimeofday( &tv, NULL ); ts = tv.tv_sec * 1000000 + tv.tv_usec;}
void TIME_TICK_END() {gettimeofday( &tv, NULL ); te = tv.tv_sec * 1000000 + tv.tv_usec;}
void TIME_TICK_PRINT(const char* T,int N=1) {printf("%s RESULT: \t%lld\t (us)\r\n", (T), (te - ts)/N );}
void TIME_TICK_ALL_PRINT(const char* T,int N=1) {printf("%s RESULT: \t%lld\t (us)\r\n", (T), (te - ts) );}

// ****************************************************************************************
// ******************************* SOBRECARGAS ********************************************
// ****************************************************************************************

// Imprimir Vector
template<typename T>
ostream& operator<<(ostream &out,const vector<T> &v){
	out<<v.size()<<' ';
	for(int i=0;i<v.size();i++) out<<v[i]<<' ';
	return out;
}

// Imprimir dato de tipo Pair
template<typename A,typename B>
ostream& operator<<(ostream &out,const pair<A,B> &p){
	out<<p.first<<' '<<p.second<<' ';
	return out;
}

// Ingresar Vector
template<typename T>
istream& operator>>(istream &in,vector<T> &v){
	int n;
	in>>n;
	v.clear();
	while(n--){
		T a;
		in>>a;
		v.push_back(a);
	}
	return in;
}

// Ingresar pair
template<typename A,typename B>
istream& operator>>(istream &in,pair<A,B> &p)
{
	in>>p.first>>p.second;
	return in;
}

// Imprimir Map
template<typename A,typename B>
ostream& operator<<(ostream &out,const map<A,B> &h){
	out<<h.size()<<' ';
	typename map<A,B>::const_iterator iter;
	for(iter=h.begin();iter!=h.end();iter++)
		out<<iter->first<<' '<<iter->second<<' ';
	return out;
}

// Ingresar Map
template<typename A,typename B>
istream& operator>>(istream &in,map<A,B> &h){
	int n;
	in>>n;
	h.clear();
	A a;B b;
	while(n--){
		in>>a>>b;
		h[a]=b;
	}
	return in;
}

// Para guardar una variable
template<typename T>void save(ostream& out,T a){
	out.write((char*)&a,sizeof(a));
}

// Para guardar un vector
template<typename T>void save(ostream& out,vector<T> &a){
	save(out,(int)a.size());
	for(int i=0;i<a.size();i++)save(out,a[i]);
}

// Para guardar un pair
template<typename A,typename B>void save(ostream &out,const pair<A,B> &p){
	save(out,p.first);save(out,p.second);
}

// Para guardar un map
template<typename A,typename B>void save(ostream &out,const map<A,B> &h){
	save(out,(int)h.size());
	typename map<A,B>::const_iterator iter;
	for(iter=h.begin();iter!=h.end();iter++){
		save(out,iter->first);
		save(out,iter->second);
	}
}

// Para leer una variable
template<typename T>void read(istream& in,T &a){
	in.read((char*)&a,sizeof(a));
}

// Para leer un vector
template<typename T>void read(istream& in,vector<T> &a){
	int n;
	read(in,n);
	a=vector<T>(n);
	for(int i=0;i<n;i++)read(in,a[i]);
}

// Para leer un pair
template<typename A,typename B>void read(istream &in,pair<A,B> &p)
{
	read(in,p.first);
	read(in,p.second);
}

// Para leer un map
template<typename A,typename B>void read(istream &in,map<A,B> &h){
	int n;
	read(in,n);
	h.clear();
	A a;B b;
	while(n--){
		read(in,a);
		read(in,b);
		h[a]=b;
	}
}

// ************************************* GRAFO ***************************************
struct Graph { // Estructura del grafo
	int n,m; // n vertices y m aristas, el numero empieza desde 0 a n-1
	int tot;
	vector<int>id; // id[i] es el número real del vértice [i]
	vector<int>head,list,next,cost; // Tabla de adyacencia
	Graph(){clear();}
	~Graph(){clear();}
	friend ostream& operator<<(ostream &out,const Graph &G){ // Almacenamiento de la estructura(stdout)
		out<<G.n<<' '<<G.m<<' '<<G.tot<<' '<<G.id<<' '<<G.head<<' '<<G.list<<' '<<G.next<<' '<<G.cost<<' ';
		return out;
	}
	friend istream& operator>>(istream &in,Graph &G){ // Instrucción de lectura(stdout)
		in>>G.n>>G.m>>G.tot>>G.id>>G.head>>G.list>>G.next>>G.cost;
		return in;
	}
	void add_D(int a,int b,int c){ // Añadir una arista a->b con peso c (dirigida)
		tot++;
		list[tot]=b;
		cost[tot]=c;
		next[tot]=head[a];
		head[a]=tot;
	}
	void add(int a,int b,int c){ // Añadir una arista con peso c en el grafo no dirigido a<->b
		add_D(a,b,c);
		add_D(b,a,c);
	}
	void init(int N,int M,int t=1){ // Inicializar grafo
		clear();
		n=N;m=M;
		tot=t;
		head=vector<int>(N);
		id=vector<int>(N);
		list=vector<int>(M*2+2);
		next=vector<int>(M*2+2);
		cost=vector<int>(M*2+2);
	}
	void clear(){ // Limpiar grafo
		n=m=tot=0;
		head.clear();
		list.clear();
		next.clear();
		cost.clear();
		id.clear();
	}
	//Partición de grafo
	vector<int>color; //01 vector de color
	vector<int>con; // Si está conectado 
	vector<int> Split(Graph *G[],int nparts){ //Particiona el grafo en dos partes, y almacena en G1, G2,2 algoritmo METIS,npart es el número de partes
		vector<int>color(n);
		int i;
		if(DEBUG1)printf("Begin-Split\n");
		if(n==nparts){
			for(i=0;i<n;i++)color[i]=i;
		}
		else{
			idx_t options[METIS_NOPTIONS];
			memset(options,0,sizeof(options));
			{
				METIS_SetDefaultOptions(options); // Inicializa el array de opciones en sus valores por defecto.
				options[METIS_OPTION_PTYPE] = METIS_PTYPE_KWAY; // _RB
				options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT; // _VOL
				options[METIS_OPTION_CTYPE] = METIS_CTYPE_SHEM; // _RM
				options[METIS_OPTION_IPTYPE] = METIS_IPTYPE_RANDOM; // _GROW _EDGE _NODE
				options[METIS_OPTION_RTYPE] = METIS_RTYPE_FM; // _GREEDY _SEP2SIDED _SEP1SIDED
				// options[METIS_OPTION_NCUTS] = 1;
				// options[METIS_OPTION_NITER] = 10;
				/* balance factor, used to be 500 */
				options[METIS_OPTION_UFACTOR] = 500;
				// options[METIS_OPTION_MINCONN];
				options[METIS_OPTION_CONTIG] = 1;
				// options[METIS_OPTION_SEED];
				options[METIS_OPTION_NUMBERING] = 0;
				// options[METIS_OPTION_DBGLVL] = 0;
			}
			idx_t nvtxs=n;
			idx_t ncon=1;
			// Transformar
			int *xadj = new idx_t[n + 1];
			int *adj=new idx_t[n+1];
			int *adjncy = new idx_t[tot-1];
			int *adjwgt = new idx_t[tot-1];
			int *part = new idx_t[n];

			int xadj_pos = 1;
			int xadj_accum = 0;
			int adjncy_pos = 0;

			// xadj, adjncy, adjwgt
			xadj[0] = 0;
			int i = 0;
			for (int i=0;i<n;i++){
				for(int j=head[i];j;j=next[j]){
					int enid = list[j];
					xadj_accum ++;
					adjncy[adjncy_pos] = enid;
					adjwgt[adjncy_pos] = cost[j];
					adjncy_pos ++;
				}
				xadj[xadj_pos++] = xadj_accum;
			}

			// Ajustamos los nodos empezando en 0
			// adjwgt -> 1
			for ( int i = 0; i < adjncy_pos; i++ ){
				adjwgt[i] = 1;
			}
			// nparts
			int objval=0;
			//METIS
			METIS_PartGraphKway( // Esta función se utiliza para particionar un grafo en n-partes utilizando el particionamiento k-way multinivel.
				&nvtxs,
				&ncon,
				xadj,
				adjncy,
				NULL,
				NULL,
				adjwgt,
				&nparts,
				NULL,
				NULL,
				options,
				&objval,
				part
			);
			for(int i=0;i<n;i++)color[i]=part[i];
			delete [] xadj;
			delete [] adj;
			delete [] adjncy;
			delete [] adjwgt;
			delete [] part;
		}
		// Partición 
		int j;
		vector<int>new_id;
		vector<int>tot(nparts,0),m(nparts,0);
		for(i=0;i<n;i++)
			new_id.push_back(tot[color[i]]++);
		for(i=0;i<n;i++)
			for(j=head[i];j;j=next[j])
				if(color[list[j]]==color[i])
					m[color[i]]++;
		for(int t=0;t<nparts;t++)
		{
			(*G[t]).init(tot[t],m[t]);
			for(i=0;i<n;i++)
				if(color[i]==t)
					for(j=head[i];j;j=next[j])
						if(color[list[j]]==color[i])
							(*G[t]).add_D(new_id[i],new_id[list[j]],cost[j]);
		}
		for(i=0;i<tot.size();i++)tot[i]=0;
		for(i=0;i<n;i++)
			(*G[color[i]]).id[tot[color[i]]++]=id[i];
		if(DEBUG1)printf("Split_over\n");
		return color;
	}
}G;

struct Matrix{ // Matrix de distancias
	Matrix():n(0),a(NULL){}
	~Matrix(){clear();}
	int n;// n*n Matriz
	int *a;
	// Salida de matrices
	friend ostream& operator<<(ostream &out,const Matrix &M){
		out<<M.n<<' ';
		for(int i=0;i<M.n;i++)
			for(int j=0;j<M.n;j++)
				out<<M.a[i*M.n+j]<<' ';
		return out;
	}
	// Entrada de Matrices
	friend istream& operator>>(istream &in,Matrix &M){
		in>>M.n;
		M.a=new int[M.n*M.n];
		for(int i=0;i<M.n;i++)
			for(int j=0;j<M.n;j++)
				in>>M.a[i*M.n+j];
		return in;
	}
	// Guardar como binario
	void save_binary(ostream &out)
	{
		save(out,n);
		for(int i=0;i<n;i++)
			for(int j=0;j<n;j++)
				save(out,a[i*n+j]);
	}
	// Leer como binario
	void read_binary(istream &in){
		read(in,n);
		a=new int[n*n];
		for(int i=0;i<n;i++)
			for(int j=0;j<n;j++)
				read(in,a[i*n+j]);
	}
	// Llena la matriz con x
	void cover(int x){
		for(int i=0;i<n;i++)
			for(int j=0;j<n;j++)
				a[i*n+j]=x;
	}
	// Inicializar
	void init(int N){
		clear();
		n=N;
		a=new int[n*n];
		for(int i=0;i<n*n;i++)a[i]=INF;
		for(int i=0;i<n;i++)a[i*n+i]=0;
	}
	// Eliminar
	void clear(){
		delete [] a;
	}
	// Usando floyd para calcular la matriz
	void floyd(){ 
		int i,j,k;
		for(k=0;k<n;k++)
			for(i=0;i<n;i++)
				for(j=0;j<n;j++)
					if(a[i*n+j]>a[i*n+k]+a[k*n+j])a[i*n+j]=a[i*n+k]+a[k*n+j];
	}
	// Calcular la matriz con el algoritmo floyd, y registrar el resultado en orden(ruta encontrada)
	void floyd(Matrix &order){ 
		int i,j,k;
		for(k=0;k<n;k++)
			for(i=0;i<n;i++)
				for(j=0;j<n;j++)
					if(a[i*n+j]>a[i*n+k]+a[k*n+j]){
						a[i*n+j]=a[i*n+k]+a[k*n+j];
						order.a[i*n+j]=k;
					}
	}
	// Sobrecarga de =
	Matrix& operator =(const Matrix &m){
		if(this!=(&m)){
			init(m.n);
			for(int i=0;i<n;i++)
				for(int j=0;j<n;j++)
					a[i*n+j]=m.a[i*n+j];
		}
		return *this;
	}
	int* operator[](int x){return a+x*n;}
};

// ***************************** ESTRUCTURA NODO *********************************************
struct Node{
	Node(){clear();}
	int part; // Número de nodos hijos
	int n,father,deep;//n: el número del subgrafo, el id del nodo padre,son[2]hijo izquierdo e hijo derecho,deep: profundidad actual
	vector<int>son;
	Graph G;// Subgrafo
	vector<int>color; // A qué subgrafo pertenece el nodo
	Matrix dist,order; // Matriz de distancia de frontera ,ruta de paso de frontera con floyd k,orden=(-1:conectado directamente)|(-2:conectado en padres)|(-3:conectado en nodos hijos)|(-INF:ninguno)
	map<int,pair<int,int> >borders; // primero:id real del borde ;segundo:<número de borde en bordes, nodos correspondientes(0~n-1)>
	vector<int>border_in_father,border_in_son,border_id,border_id_innode;//
	// Número de bordes en el nodo padre y en el nodo hijo, id bruto en el grafo G, número de bordes en el subnodo
	vector<int>path_record; //Registro temporal de la ruta
	int cache_vertex_id,cache_bound;// El id de inicio almacenado en caché, el límite actualizado en caché(bound had updated end)
	vector<int>cache_dist;
	//cache_dist almacena la distancia desde cache_id a todos los bordes, sólo será correcta cuando sea menor o igual que cache_bound
	vector<int>border_son_id; // El número del nodo hijo al que pertenece la frontera   
	int min_border_dist; // distancia en caché de la frontera actual en la caché (para corte KNN)
	vector<pair<int,int> >min_car_dist; // El coche más cercano y el node_id a la frontera <car_dist,node_id>

	// Salidad del Nodo
	friend ostream& operator<<(ostream &out,const Node &N){
		out<<N.n<<' '<<N.father<<' '<<N.part<<' '<<N.deep<<' '<<N.cache_vertex_id<<' '<<N.cache_bound<<' '<<N.min_border_dist<<' ';
		for(int i=0;i<N.part;i++)out<<N.son[i]<<' ';
		if(INDEX_Optimization){
    		out<<N.color<<' '<<N.dist<<' '<<N.borders<<' ';
    		if(Save_Order_Matrix)out<<N.order<<' ';
    	}
		else{
    		out<<N.color<<' '<<N.dist<<' '<<N.borders<<' '<<N.border_in_father<<' '<<N.border_in_son<<' '<<N.border_id<<' '<<N.border_id_innode<<' '<<N.path_record<<' '<<N.cache_dist<<' '<<N.min_car_dist<<' '<<N.border_son_id<<' ';
    	    if(Save_Order_Matrix)out<<N.order<<' ';
    	}
		return out;		
	}
	friend istream& operator>>(istream &in,Node &N){
		in>>N.n>>N.father>>N.part>>N.deep>>N.cache_vertex_id>>N.cache_bound>>N.min_border_dist;
		N.son.clear();
		N.son=vector<int>(N.part);
		for(int i=0;i<N.part;i++)in>>N.son[i];
		if(INDEX_Optimization){
    		in>>N.color>>N.dist>>N.borders;
    		if(Save_Order_Matrix)in>>N.order;
    	}
		else{
		    in>>N.color>>N.dist>>N.borders>>N.border_in_father>>N.border_in_son>>N.border_id>>N.border_id_innode>>N.path_record>>N.cache_dist>>N.min_car_dist>>N.border_son_id;
    		if(Save_Order_Matrix)in>>N.order;
		}
		return in;		
	}
	void save_binary(ostream &out){
		save(out,n);
		save(out,father);
		save(out,part);
		save(out,deep);
		save(out,cache_vertex_id);
		save(out,cache_bound);
		save(out,min_border_dist);
		for(int i=0;i<part;i++)save(out,son[i]);
		if(INDEX_Optimization){
			save(out,color);
			dist.save_binary(out);
			if(Save_Order_Matrix)order.save_binary(out);
			save(out,borders);
		}
		else{
			save(out,color);
			dist.save_binary(out);
			if(Save_Order_Matrix)order.save_binary(out);
			save(out,borders);
			save(out,border_in_father);
			save(out,border_in_son);
			save(out,border_id);
			save(out,border_id_innode);
			save(out,path_record);
			save(out,cache_dist);
			save(out,min_car_dist);
			save(out,border_son_id);
		}		
	}
	void read_binary(istream &in)
	{
		read(in,n);
		read(in,father);
		read(in,part);
		read(in,deep);
		read(in,cache_vertex_id);
		read(in,cache_bound);
		read(in,min_border_dist);
		son.clear();
		son=vector<int>(part);
		for(int i=0;i<part;i++)read(in,son[i]);
		//printf("read_binary Node n=%d father=%d part=%d deep=%d\n",n,father,part,deep);
		if(INDEX_Optimization){
			read(in,color);
			dist.read_binary(in);
			if(Save_Order_Matrix)order.read_binary(in);
			read(in,borders);
		}
		else{
			read(in,color);
			dist.read_binary(in);
			if(Save_Order_Matrix)order.read_binary(in);
			read(in,borders);
			read(in,border_in_father);
			read(in,border_in_son);
			read(in,border_id);
			read(in,border_id_innode);
			read(in,path_record);
			read(in,cache_dist);
			read(in,min_car_dist);
			read(in,border_son_id);
		}
	}
	void init(int n){
		part=n;
		son=vector<int>(n);
		for(int i=0;i<n;i++)son[i]=0;
	}
	void clear(){
		part=n=father=deep=0;
		son.clear();
		dist.clear();
		order.clear();
		G.clear();
		color.clear();
		borders.clear();
		border_in_father.clear();
		border_in_son.clear();
		border_id.clear();
		border_id_innode.clear();
		cache_dist.clear();
		cache_vertex_id=-1;
		path_record.clear();
		cache_dist.clear();
		border_son_id.clear();
		min_car_dist.clear();
	}
	void make_border_edge(){ // Actualiza la arista directamente conectada del borde a dist(build_dist1)
		int i,j;
		map<int,pair<int,int> >::iterator iter;
		for(iter=borders.begin();iter!=borders.end();iter++){
			i=iter->second.second;
			for(j=G.head[i];j;j=G.next[j])
				if(color[i]!=color[G.list[j]]){
					int id1,id2;
					id1=iter->second.first;
					id2=borders[G.id[G.list[j]]].first;
					if(dist[id1][id2]>G.cost[j]){
						dist[id1][id2]=G.cost[j];
						order[id1][id2]=-1;
					}
				}
		}
	}
};

// *********************************************
// ESTRUCTURA G_Tree
// *********************************************
struct G_Tree{
	int root;
	vector<int>id_in_node;			//a que nodo pertenece el vértice
	vector<vector<int> >car_in_node;//Registra los vehículos en el vértice.
    vector<int>car_offset; 			//El desplazamiento del vehículo en el vértice.

	struct Interface{
		G_Tree *tree;               //Puntero al G_Tree(var)
		int tot,size,id_tot;        //tot: límite superior del subíndice vir, tamaño: tamaño del vector, id_tot límite superior de la memoria
		int node_size;              //tamaño del nodo vector
		Node *node;
		vector<int>id;              //id del nodo en el vector id
		//sub node informacion
		vector<int>father;          //nodo padre del nodo vir
		vector<int>border_in_father;//nodo vir border_in_father
		vector<int>G_id;            //real id de la hoja con n=1 in node[0]

        //Incializar node[0] con x
        void build_node0(int x)
		{
			//cnt2++;
			node[0].borders.clear();
			node[0].borders[G_id[x]]=make_pair(0,0);
			node[0].border_id[0]=G_id[x];
			node[0].border_in_father[0]=border_in_father[x];
			node[0].father=father[x];
			node[0].deep=node[id[father[x]]].deep+1;
			{
				int node_id=G_id[x];
				if(tree->car_in_node[node_id].size()>0)
					node[0].min_car_dist[0]=make_pair(0,node_id);
				else
					node[0].min_car_dist[0]=make_pair(INF,-1);
			}

		}

		Node& operator[](int x){
			if(id[x]==0 && Memory_Optimization)
				build_node0(x);
			return node[id[x]];
		}
		friend ostream& operator<<(ostream &out,Interface &I)
		{
			out<<I.tot<<' '<<I.size<<' '<<I.id_tot<<' '<<I.node_size<<' '<<Save_Order_Matrix<<' ';
			for(int i=0;i<I.node_size;i++)out<<I.node[i]<<' ';
			out<<I.id<<' '<<I.father<<' '<<I.border_in_father<<' '<<I.G_id<<' ';
			return out;
		}
		friend istream& operator>>(istream &in,Interface &I)
		{
			in>>I.tot>>I.size>>I.id_tot>>I.node_size>>Save_Order_Matrix;
			delete []I.node;
			I.node=new Node[I.node_size];
			for(int i=0;i<I.node_size;i++)in>>I.node[i];
			in>>I.id>>I.father>>I.border_in_father>>I.G_id;
			//Initialize node[0]
			I.node[0].border_id.clear();
			I.node[0].border_id.push_back(0);
			I.node[0].border_in_father.clear();
			I.node[0].border_in_father.push_back(0);
			I.node[0].min_car_dist.clear();
			I.node[0].min_car_dist.push_back(make_pair(0,0));
			return in;
		}
		void save_binary(ostream &out)
		{
			save(out,tot);
			save(out,size);
			save(out,id_tot);
			save(out,node_size);
			save(out,Save_Order_Matrix);
			for(int i=0;i<node_size;i++)
			{
			    node[i].save_binary(out);
			}
			save(out,id);
			save(out,father);
			save(out,border_in_father);
			save(out,G_id);
		}
		void read_binary(istream &in)
		{
			read(in,tot);
			read(in,size);
			read(in,id_tot);
			read(in,node_size);
			read(in,Save_Order_Matrix);
			delete []node;
			node=new Node[node_size];
			for(int i=0;i<node_size;i++)
			{
			    node[i].read_binary(in);
			}
			read(in,id);
			read(in,father);
			read(in,border_in_father);
			read(in,G_id);
			//initialize  node[0]
			node[0].border_id.clear();
			node[0].border_id.push_back(0);
			node[0].border_in_father.clear();
			node[0].border_in_father.push_back(0);
			node[0].min_car_dist.clear();
			node[0].min_car_dist.push_back(make_pair(0,0));
		}
		~Interface(){delete [] node;}
	}node;
	friend ostream& operator<<(ostream &out,G_Tree &T)
	{
		out<<T.root<<' '<<T.id_in_node<<' '<<T.car_in_node<<' '<<T.car_offset<<' ';
		out<<T.node<<' ';
		return out;
	}
	friend istream& operator>>(istream &in,G_Tree &T)
	{
		printf("load_begin\n");fflush(stdout);
		in>>T.root>>T.id_in_node>>T.car_in_node>>T.car_offset;
		in>>T.node;
		T.node.tree=&T;
		if(INDEX_Optimization)
		{
			printf("build_border_in_father_son\n");fflush(stdout);
			T.build_border_in_father_son();
		}
		return in;
	}
	void save_binary(ostream &out)
	{
		save(out,root);
		save(out,id_in_node);
		save(out,car_in_node);
		save(out,car_offset);
		node.save_binary(out);
	}
	void read_binary(istream &in)
	{
		read(in,root);
		read(in,id_in_node);
		read(in,car_in_node);
		read(in,car_offset);
		node.read_binary(in);
		node.tree=this;
		if(INDEX_Optimization)
		{
			printf("build_border_in_father_son\n");fflush(stdout);
			build_border_in_father_son();
		}
	}

    // agrega un id real al conjunto de límites de x, en vir es id2, su correspondiente id de borde
    void add_border(int x,int id,int id2)
	{
		map<int,pair<int,int> >::iterator iter;
		iter=node[x].borders.find(id);
		if(iter==node[x].borders.end())
		{
			pair<int,int> second=make_pair((int)node[x].borders.size(),id2);
			node[x].borders[id]=second;
		}
	}

    //calcular el conjunto de x
	void make_border(int x,const vector<int> &color)//calculate the set of x
	{
		for(int i=0;i<node[x].G.n;i++)
		{
			int id=node[x].G.id[i];
			for(int j=node[x].G.head[i];j;j=node[x].G.next[j])
				if(color[i]!=color[node[x].G.list[j]])
				{
					add_border(x,id,i);
					break;
				}
		}
	}

    // Construir árbol, x actual, número de subgráfos f, subgráfo actual g
	void build(int x=1,int f=1,const Graph &g=G)
	{
		if(x==1)//x es raiz 
		{
			node.tree=this;
			node.size=G.n*2+2;
			printf("malloc!\n");fflush(stdout);
			node.node=new Node[node.size];
			printf("malloc end!\n");fflush(stdout);
			node.id=vector<int>(node.size);
			for(int i=0;i<node.size;i++)node.id[i]=i;
			node.tot=2;
			root=1;
			node[x].deep=1;
			node[1].G=g;
			node.node_size=node.size;
		}
		else//x no es raiz
		{
			node[x].deep=node[node[x].father].deep+1;
		}
		node[x].n=node[x].G.n;
		if(node[x].G.n<Naive_Split_Limit)node[x].init(node[x].n);
		else node[x].init(Partition_Part);
		if(node[x].n>50)printf("x=%d deep=%d n=%d ",x,node[x].deep,node[x].G.n);
		if(node[x].n>f)
		{
			//id del subnodo
			int top=node.tot;
			for(int i=0;i<node[x].part;i++)
			{
				node[x].son[i]=top+i;
				node[top+i].father=x;
			}
			node.tot+=node[x].part;
			//añadir borde entre dos grafos
			Graph **graph;
			graph=new Graph*[node[x].part];
			for(int i=0;i<node[x].part;i++)graph[i]=&node[node[x].son[i]].G;
			node[x].color=node[x].G.Split(graph,node[x].part);
			delete [] graph;
			make_border(x,node[x].color);
			if(node[x].n>50)printf("border=%d\n",node[x].borders.size());
			//dar el valor de borde al subgrafo 
			map<int,pair<int,int> >::iterator iter;
			for(iter=node[x].borders.begin();iter!=node[x].borders.end();iter++)
			{
				//printf("(%d,%d,%d)",iter->first,iter->second.first,iter->second.second);
				node[x].color[iter->second.second]=-node[x].color[iter->second.second]-1;
			}
			//cout<<endl;
			vector<int>tot(node[x].part,0);
			for(int i=0;i<node[x].n;i++)
			{
				if(node[x].color[i]<0)
				{
					node[x].color[i]=-node[x].color[i]-1;
					add_border(node[x].son[node[x].color[i]],node[x].G.id[i],tot[node[x].color[i]]);
				}
				tot[node[x].color[i]]++;
			}
			//iterar subnode
			for(int i=0;i<node[x].part;i++)
				build(node[x].son[i]);
		}
		else if(node[x].n>50)cout<<endl;
		node[x].dist.init(node[x].borders.size());
		node[x].order.init(node[x].borders.size());
		node[x].order.cover(-INF);
		if(x==1)//construir dist por raíz de x
		{
			for(int i=1;i<min(1000,node.tot-1);i++)
				if(node[i].n>50)
				{
					printf("x=%d deep=%d n=%d ",i,node[i].deep,node[i].G.n);
				}
			printf("begin_build_border_in_father_son\n");
			build_border_in_father_son();
			printf("begin_build_dist\n");
			build_dist1(root);
			printf("begin_build_dist2\n");
			build_dist2(root);
			//calcular el id del nodo hoja del vértice en 
			id_in_node.clear();
			for(int i=0;i<node[root].G.n;i++)id_in_node.push_back(-1);
			for(int i=1;i<node.tot;i++)
				if(node[i].G.n==1)
					id_in_node[node[i].G.id[0]]=i;
			{
				//car_in_node;
				vector<int>empty_vector;
				empty_vector.clear();
				car_in_node.clear();
				for(int i=0;i<G.n;i++)car_in_node.push_back(empty_vector);
			}
			if(Memory_Optimization)Memory_Compress();
		}
	}

    // de abajo hacia arriba fusionar dist en Graph
    void build_dist1(int x=1)
	{
		// Calcular dist en subgrafo y poner a x
		for(int i=0;i<node[x].part;i++)if(node[x].son[i])build_dist1(node[x].son[i]);
		if(node[x].son[0])//not leaf
		{
			//construir la arista entre x subnodo
			node[x].make_border_edge();
			// construir la dist real en el interior
			node[x].dist.floyd(node[x].order);
		}
		else ;//hoja
		// dar el peso interior al padre
		if(node[x].father)
		{
			int y=node[x].father,i,j;
			map<int,pair<int,int> >::iterator x_iter1,y_iter1;
			vector<int>id_in_fa(node[x].borders.size());
			//calcular el id de borde en padre borde, no existe:-1
			for(x_iter1=node[x].borders.begin();x_iter1!=node[x].borders.end();x_iter1++)
			{
				y_iter1=node[y].borders.find(x_iter1->first);
				if(y_iter1==node[y].borders.end())id_in_fa[x_iter1->second.first]=-1;
				else id_in_fa[x_iter1->second.first]=y_iter1->second.first;
			}
			//dice el peso de todas las conexiones al padre
			for(i=0;i<(int)node[x].borders.size();i++)
				for(j=0;j<(int)node[x].borders.size();j++)
					if(id_in_fa[i]!=-1&&id_in_fa[j]!=-1)
					{
						int *p=&node[y].dist[id_in_fa[i]][id_in_fa[j]];
						if((*p)>node[x].dist[i][j])
						{
							(*p)=node[x].dist[i][j];
							node[y].order[id_in_fa[i]][id_in_fa[j]]=-3;
						}
					}
		}
		return;
	}

    //Calcular de arriba hacia abajo como en paper
	void build_dist2(int x=1)
	{
		if(x!=root)node[x].dist.floyd(node[x].order);
		if(node[x].son[0])
		{
			//calcular el id del borde en el subgrafo del borde actual 
			vector<int>id_(node[x].borders.size());
			vector<int>color_(node[x].borders.size());
			map<int,pair<int,int> >::iterator iter1,iter2;
			for(iter1=node[x].borders.begin();iter1!=node[x].borders.end();iter1++)
			{
				int c=node[x].color[iter1->second.second];
				color_[iter1->second.first]=c;
				int y=node[x].son[c];
				id_[iter1->second.first]=node[y].borders[iter1->first].first;
			}
			//Recalcular el peso del subgrafo 
			for(int i=0;i<(int)node[x].borders.size();i++)
				for(int j=0;j<(int)node[x].borders.size();j++)
					if(color_[i]==color_[j])
					{
						int y=node[x].son[color_[i]];
						int *p=&node[y].dist[id_[i]][id_[j]];
						if((*p)>node[x].dist[i][j])
						{
							(*p)=node[x].dist[i][j];
							node[y].order[id_[i]][id_[j]]=-2;
						}
					}
			// Subnodos recursivos 
			for(int i=0;i<node[x].part;i++)
				if(node[x].son[i])build_dist2(node[x].son[i]);
		}
	}

    //calcular el id del borde en padre/hijo
    void build_border_in_father_son()
	{
		int i,j,x,y;
		//Construir cache
		for(x=1;x<node.tot;x++)
		{
			//printf("x=%d node.id[x]=%d size=%d\n",x,node.id[x],node.node_size);fflush(stdout);
			//node[x].write();fflush(stdout);
			node[x].cache_dist.clear();
			node[x].min_car_dist.clear();
			node[x].cache_vertex_id=-1;
			for(int j=0;j<node[x].borders.size();j++)
			{
				node[x].cache_dist.push_back(0);
				node[x].min_car_dist.push_back(make_pair(INF,-1));
			}
			node[x].border_id.clear();
			node[x].border_id_innode.clear();
			for(i=0;i<node[x].borders.size();i++)node[x].border_id.push_back(0);
			for(i=0;i<node[x].borders.size();i++)node[x].border_id_innode.push_back(0);
			for(map<int,pair<int,int> >::iterator iter=node[x].borders.begin();iter!=node[x].borders.end();iter++)
			{
				node[x].border_id[iter->second.first]=iter->first;
				node[x].border_id_innode[iter->second.first]=iter->second.second;
			}
			node[x].border_in_father.clear();
			node[x].border_in_son.clear();
			for(i=0;i<node[x].borders.size();i++)
			{
				node[x].border_in_father.push_back(-1);
				node[x].border_in_son.push_back(-1);
			}
			if(node[x].father)
			{
				y=node[x].father;
				map<int,pair<int,int> >::iterator iter;
				for(iter=node[x].borders.begin();iter!=node[x].borders.end();iter++)
				{
					map<int,pair<int,int> >::iterator iter2;
					iter2=node[y].borders.find(iter->first);
					if(iter2!=node[y].borders.end())
						node[x].border_in_father[iter->second.first]=iter2->second.first;
				}
			}
			if(node[x].son[0])
			{
				map<int,pair<int,int> >::iterator iter;
				for(iter=node[x].borders.begin();iter!=node[x].borders.end();iter++)
				{
					y=node[x].son[node[x].color[iter->second.second]];
					map<int,pair<int,int> >::iterator iter2;
					iter2=node[y].borders.find(iter->first);
					if(iter2!=node[y].borders.end())
						node[x].border_in_son[iter->second.first]=iter2->second.first;
				}
				node[x].border_son_id.clear();
				for(int i=0;i<node[x].borders.size();i++)
					node[x].border_son_id.push_back(node[x].son[node[x].color[node[x].border_id_innode[i]]]);
			}
		}
	}

    //Comprimir el árbol en memoria hacer el nodo con n=1 visual
	void Memory_Compress()
	{
		printf("Begin Memory Compress! node_size=%d\n",node.node_size);
		int cnt=0;
		for(int i=0;i<node.tot;i++)
			if(i==0 || node.node[i].n>1)cnt++;
		Node *new_node=new Node[cnt+1];
		node.node_size=cnt+1;
		node.id_tot=1;
		node.border_in_father=node.G_id=node.father=vector<int>(node.tot);
		for(int i=0;i<node.tot;i++)
		{
			if(node.node[i].n>1 || i==0)
			{
				node.id[i]=(node.id_tot++);
				new_node[node.id[i]]=node.node[i];
			}
			else 
			{
				//node.node[i].write();
				new_node[node.id[i]=0]=node.node[i];
				node.G_id[i]=node.node[i].G.id[0];
				node.father[i]=node.node[i].father;
				node.border_in_father[i]=node.node[i].border_in_father[0];
			}
		}
		delete []node.node;
		node.node=new_node;
		printf("End Memory Compress! node_size=%d\n",node.node_size);fflush(stdout);
	}

    //coloque la distancia más corta de S a x en dist1, luego calcule la distancia de S a x.padre y actualice dist
    //tipo = 0 está arriba, tipo = 1 está abajo
	void push_borders_up(int x,vector<int> &dist1,int type)
	{
		if(node[x].father==0)return;
		int y=node[x].father;
		vector<int>dist2(node[y].borders.size(),INF);
		for(int i=0;i<node[x].borders.size();i++)
			if(node[x].border_in_father[i]!=-1)
				dist2[node[x].border_in_father[i]]=dist1[i];
		Matrix &dist=node[y].dist;
		int *begin,*end;
		begin=new int[node[x].borders.size()];
		end=new int[node[y].borders.size()];
		int tot0=0,tot1=0;
		for(int i=0;i<dist2.size();i++)
		{
			if(dist2[i]<INF)begin[tot0++]=i;
			else if(node[y].border_in_father[i]!=-1)end[tot1++]=i;
		}
		if(type==0)
		{
			for(int i=0;i<tot0;i++)
			{
				int i_=begin[i];
				for(int j=0;j<tot1;j++)
				{
					int j_=end[j];
					if(dist2[j_]>dist2[i_]+dist[i_][j_])
					dist2[j_]=dist2[i_]+dist[i_][j_];
				}
			}
		}
		else{
			for(int i=0;i<tot0;i++)
			{
				int i_=begin[i];
				for(int j=0;j<tot1;j++)
				{
					int j_=end[j];
					if(dist2[j_]>dist2[i_]+dist[j_][i_])
					dist2[j_]=dist2[i_]+dist[j_][i_];
				}
			}
		}
		dist1=dist2;
		delete [] begin;
		delete [] end;
	}

    //cachear S al vértice en x.cache_dist, y calcular la distancia de S a x.father, y actualizar x.father.cache
    void push_borders_up_cache(int x,int bound=INF)
	{
		if(node[x].father==0)return;
		int y=node[x].father;
		if(node[x].cache_vertex_id==node[y].cache_vertex_id&&bound<=node[y].cache_bound)return;
		node[y].cache_vertex_id=node[x].cache_vertex_id;
		node[y].cache_bound=bound;
		vector<int> *dist1=&node[x].cache_dist,*dist2=&node[y].cache_dist;
		for(int i=0;i<(*dist2).size();i++)(*dist2)[i]=INF;
		for(int i=0;i<node[x].borders.size();i++)
			if(node[x].border_in_father[i]!=-1)
			{
				if(node[x].cache_dist[i]<bound)//begin in the bound
					(*dist2)[node[x].border_in_father[i]]=(*dist1)[i];
				else (*dist2)[node[x].border_in_father[i]]=-1;//begin out of the bound
			}
		Matrix &dist=node[y].dist;
		int *begin,*end;// calculated id and un-calculated id  
		begin=new int[node[x].borders.size()];
		end=new int[node[y].borders.size()];
		int tot0=0,tot1=0;
		for(int i=0;i<(*dist2).size();i++)
		{
			if((*dist2)[i]==-1)(*dist2)[i]=INF;
			else if((*dist2)[i]<INF)begin[tot0++]=i;
			else if(node[y].border_in_father[i]!=-1)
				end[tot1++]=i;
		}
		for(int i=0;i<tot0;i++)
		{
			int i_=begin[i];
			for(int j=0;j<tot1;j++)
			{
				if((*dist2)[end[j]]>(*dist2)[i_]+dist[i_][end[j]])
				(*dist2)[end[j]]=(*dist2)[i_]+dist[i_][end[j]];
			}
		}
		delete [] begin;
		delete [] end;
		node[y].min_border_dist=INF;
		for(int i=0;i<node[y].cache_dist.size();i++)
			if(node[y].border_in_father[i]!=-1)
				node[y].min_border_dist=min(node[y].min_border_dist,node[y].cache_dist[i]);
	}

    //calcular la distancia de S al hijo de x, actualizar y.cache por S a los bordes en x.cache_dist
	void push_borders_down_cache(int x,int y,int bound=INF)
	{
		if(node[x].cache_vertex_id==node[y].cache_vertex_id&&bound<=node[y].cache_bound)return;
		node[y].cache_vertex_id=node[x].cache_vertex_id;
		node[y].cache_bound=bound;
		vector<int> *dist1=&node[x].cache_dist,*dist2=&node[y].cache_dist;
		for(int i=0;i<(*dist2).size();i++)(*dist2)[i]=INF;
		for(int i=0;i<node[x].borders.size();i++)
			if(node[x].son[node[x].color[node[x].border_id_innode[i]]]==y)
			{
				if(node[x].cache_dist[i]<bound)//bound in ,begin
					(*dist2)[node[x].border_in_son[i]]=(*dist1)[i];
				else (*dist2)[node[x].border_in_son[i]]=-1;//begin out of bound
			}
		Matrix &dist=node[y].dist;
		int *begin,*end;// calculated id and un-calculated id 
		begin=new int[node[y].borders.size()];
		end=new int[node[y].borders.size()];
		int tot0=0,tot1=0;
		for(int i=0;i<(*dist2).size();i++)
		{
			if((*dist2)[i]==-1)(*dist2)[i]=INF;
			else if((*dist2)[i]<INF)begin[tot0++]=i;
			else end[tot1++]=i;
		}
		for(int i=0;i<tot0;i++)
		{
			int i_=begin[i];
			for(int j=0;j<tot1;j++)
			{
				if((*dist2)[end[j]]>(*dist2)[i_]+dist[i_][end[j]])
				(*dist2)[end[j]]=(*dist2)[i_]+dist[i_][end[j]];
			}
		}
		delete [] begin;
		delete [] end;
		node[y].min_border_dist=INF;
		for(int i=0;i<node[y].cache_dist.size();i++)
			if(node[y].border_in_father[i]!=-1)
				node[y].min_border_dist=min(node[y].min_border_dist,node[y].cache_dist[i]);
	}

    //desde S hasta el vértice x almacenado en x.cache_dist, calcule la distancia desde S hasta el vértice hermano y y actualice y.cache
    void push_borders_brother_cache(int x,int y,int bound=INF)
	{
		int S=node[x].cache_vertex_id,LCA=node[x].father,i,j;
		if(node[y].cache_vertex_id==S&&node[y].cache_bound>=bound)return;
		int p;
		node[y].cache_vertex_id=S;
		node[y].cache_bound=bound;
		vector<int>id_LCA[2],id_now[2];
		//id of Candidate border in LCA, id of the Candidate border inside border 
		for(int t=0;t<2;t++)
		{
			if(t==0)p=x;
			else p=y;
			for(i=j=0;i<(int)node[p].borders.size();i++)
				if(node[p].border_in_father[i]!=-1)
					if((t==1)||(t==0&&node[p].cache_dist[i]<bound)){
						id_LCA[t].push_back(node[p].border_in_father[i]);
						id_now[t].push_back(i);
					}
		}
		for(int i=0;i<node[y].cache_dist.size();i++)node[y].cache_dist[i]=INF;
		for(int i=0;i<id_LCA[0].size();i++)
			for(int j=0;j<id_LCA[1].size();j++){
				int k=node[x].cache_dist[id_now[0][i]]+node[LCA].dist[id_LCA[0][i]][id_LCA[1][j]];
				if(k<node[y].cache_dist[id_now[1][j]])node[y].cache_dist[id_now[1][j]]=k;
			}
		Matrix &dist=node[y].dist;
		int *begin,*end;// calculated id and un-calculated id 
		begin=new int[node[y].borders.size()];
		end=new int[node[y].borders.size()];
		int tot0=0,tot1=0;
		for(int i=0;i<node[y].cache_dist.size();i++){
			if(node[y].cache_dist[i]<bound)begin[tot0++]=i;
			else if(node[y].cache_dist[i]==INF)
				end[tot1++]=i;
		}
		for(int i=0;i<tot0;i++){
			int i_=begin[i];
			for(int j=0;j<tot1;j++){
				if(node[y].cache_dist[end[j]]>node[y].cache_dist[i_]+dist[i_][end[j]])
				node[y].cache_dist[end[j]]=node[y].cache_dist[i_]+dist[i_][end[j]];
			}
		}
		delete [] begin;
		delete [] end;
		node[y].min_border_dist=INF;
		for(int i=0;i<node[y].cache_dist.size();i++)
			if(node[y].border_in_father[i]!=-1)
				node[y].min_border_dist=min(node[y].min_border_dist,node[y].cache_dist[i]);
	}

    // Cache S al borde de x en dist1
    //calcular la distancia de s a x.padre, actualizar dist1
    //grabar x.padre a x.padre.path_record(>=0 significa nodo, <0 significa de dónde viene, -INF no pre)
    void push_borders_up_path(int x,vector<int> &dist1)
	{
		if(node[x].father==0)return;
		int y=node[x].father;
		vector<int>dist3(node[y].borders.size(),INF);
		vector<int> *order=&node[y].path_record;
		(*order).clear();
		for(int i=0;i<node[y].borders.size();i++)(*order).push_back(-INF);
		for(int i=0;i<node[x].borders.size();i++)
			if(node[x].border_in_father[i]!=-1)
			{
				dist3[node[x].border_in_father[i]]=dist1[i];
				(*order)[node[x].border_in_father[i]]=-x;
			}
		//printf("dist3:");save_vector(dist3);
		Matrix &dist=node[y].dist;
		int *begin,*end;// calculated id and un-calculated id 
		begin=new int[node[x].borders.size()];
		end=new int[node[y].borders.size()];
		int tot0=0,tot1=0;
		for(int i=0;i<dist3.size();i++)
		{
			if(dist3[i]<INF)begin[tot0++]=i;
			else if(node[y].border_in_father[i]!=-1)end[tot1++]=i;
		}
		for(int i=0;i<tot0;i++)
		{
			int i_=begin[i];
			for(int j=0;j<tot1;j++)
			{
				if(dist3[end[j]]>dist3[i_]+dist[i_][end[j]])
				{
					dist3[end[j]]=dist3[i_]+dist[i_][end[j]];
					(*order)[end[j]]=i_;
				}
			}
		}
		dist1=dist3;
		delete [] begin;
		delete [] end;
	}

    //LCA de x y y
    int find_LCA(int x,int y)
	{
		if(node[x].deep<node[y].deep)swap(x,y);
		while(node[x].deep>node[y].deep)x=node[x].father;
		while(x!=y){x=node[x].father;y=node[y].father;}
		return x;
	}

    //La distancia más corta de S a T
	int search(int S,int T){
		if(S==T)return 0;
		//Calculate LCA
		int i,j,k,p;
		int LCA,x=id_in_node[S],y=id_in_node[T];
		if(node[x].deep<node[y].deep)swap(x,y);
		while(node[x].deep>node[y].deep)x=node[x].father;
		while(x!=y){x=node[x].father;y=node[y].father;}
		LCA=x;
		vector<int>dist[2],dist_;
		dist[0].push_back(0);
		dist[1].push_back(0);
		x=id_in_node[S],y=id_in_node[T];
		//naive method as GTree
		for(int t=0;t<2;t++)
		{
			if(t==0)p=x;
			else p=y;
			while(node[p].father!=LCA)
			{
				push_borders_up(p,dist[t],t);
				p=node[p].father;
			}
			if(t==0)x=p;
			else y=p;
		}
		vector<int>id[2];//id de frontera actual en la secuencia del vector de frontera LCA 
		for(int t=0;t<2;t++)
		{
			if(t==0)p=x;
			else p=y;
			for(i=j=0;i<(int)dist[t].size();i++)
				if(node[p].border_in_father[i]!=-1)
				{
					id[t].push_back(node[p].border_in_father[i]);
					dist[t][j]=dist[t][i];
					j++;
				}
			while(dist[t].size()>id[t].size()){dist[t].pop_back();}
		}
		//matched - emparejado
		int MIN=INF;
		for(i=0;i<dist[0].size();i++)
		{
			int i_=id[0][i];
			for(j=0;j<dist[1].size();j++)
			{
				k=dist[0][i]+dist[1][j]+node[LCA].dist[i_][id[1][j]];
				if(k<MIN)MIN=k;
			}
		}
		return MIN;
	}

    //Buscar la distancia más corta de S a T, y almacenarlos en caché, omitir peso> = límite, si no, devolver INF
    int search_cache(int S,int T,int bound=INF)
	{
		//Calculo ingenuo y cache
		if(S==T)return 0;
		//LCA
		int i,j,k,p;
		int x=id_in_node[S],y=id_in_node[T];
		int LCA=find_LCA(x,y);

		//Get LCA path of two nodes 
		vector<int>node_path[2];// the id before x/y arrived LCA
		for(int t=0;t<2;t++)
		{
			if(t==0)p=x;
			else p=y;
			while(node[p].father!=LCA)
			{
				node_path[t].push_back(p);
				p=node[p].father;
			}
			node_path[t].push_back(p);
			if(t==0)x=p;
			else y=p;
		}

		//push from leaf to the next level of LCA
		node[id_in_node[S]].cache_vertex_id=S;
		node[id_in_node[S]].cache_bound=bound;
		node[id_in_node[S]].min_border_dist=0;
		node[id_in_node[S]].cache_dist[0]=0;
		for(i=0;i+1<node_path[0].size();i++)
		{
			if(node[node_path[0][i]].min_border_dist>=bound)return INF;
			push_borders_up_cache(node_path[0][i]);
		}

		// calculate T distance in the layer under LCA and cache
		if(node[x].min_border_dist>=bound)return INF;
		push_borders_brother_cache(x,y);
		// push T under LCA to bottom node T
		for(int i=node_path[1].size()-1;i>0;i--)
		{
			if(node[node_path[1][i]].min_border_dist>=bound)return INF;
			push_borders_down_cache(node_path[1][i],node_path[1][i-1]);
		}

		//Final Result
		return node[id_in_node[T]].cache_dist[0];
	}

    //* Agregar un auto car_id que pertenezca a node_id al conjunto de autos.
	void add_car(int node_id,int car_id)
	{
		car_in_node[node_id].push_back(car_id);
		if(car_in_node[node_id].size()==1)
		{
			 // add node_id and update border LNAV
			int S=id_in_node[node_id];  //get the id of leaf node which it in      
			node[S].min_car_dist[0]=make_pair(0,node_id); // make pair for LNAV
			for(int p=S;push_borders_up_add_min_car_dist(p,node_id);p=node[p].father);  //add it to some node
		}
	}

    //* Eliminar un coche car_id que pertenece a node_id del conjunto de coches
    void del_car(int node_id,int car_id)
	{
		int i;
		for(i=0;i<car_in_node[node_id].size();i++)
			if(car_in_node[node_id][i]==car_id)break;
		if(i==car_in_node[node_id].size())printf("Error: del_car find none car!");
		while(i+1<car_in_node[node_id].size())
        {
            swap(car_in_node[node_id][i],car_in_node[node_id][i+1]);
            i++;

        } //drop all in car_in_node  

		car_in_node[node_id].pop_back();
		if(car_in_node[node_id].size()==0)
		{
			// delete from the LNAV
			int S=id_in_node[node_id];
			node[S].min_car_dist[0]=make_pair(INF,-1);//INF,-1 Set to INF, -1
			for(int p=S;push_borders_up_del_min_car_dist(p,node_id);p=node[p].father);
		}
        //
	}

	//* Actualizar el desplazamiento de car_id al vértice
    void change_car_offset(int car_id,int dist)
	{
		while(car_offset.size()<=car_id)car_offset.push_back(0);
		car_offset[car_id]=dist;
	}

    //* Obtener el desplazamiento de car_id
	int get_car_offset(int car_id)
	{
		while(car_offset.size()<=car_id)car_offset.push_back(0);
		return car_offset[car_id];
	}

	//push_borders_up_add/del_min_car_dist vector de vértice calculado y no calculado
    int begin[10000],end[10000];
    int buffer[10000];


    //* actualizar x.father por min_car_dist en x, actualice donde node_id=start_id, si no existe, devuelve falso, o verdadero.
    bool push_borders_up_add_min_car_dist(int x,int start_id)
	{
		// leaf node and the position of the car
		int re=false;
		if(node[x].father==0)return re;
		int y=node[x].father;
		vector<pair<int,int> > &dist1=node[x].min_car_dist,&dist2=node[y].min_car_dist;
		int nodex_borders_size=node[x].borders.size();
		vector<int> &nodex_border_in_father=node[x].border_in_father;
		for(int i=0;i<nodex_borders_size;i++){
            if(dist1[i].second==start_id) //border with new comer
            {
                re=true; //true 
                if(nodex_border_in_father[i]!=-1)//if border belongs to its father, update
                {
                    if(dist2[nodex_border_in_father[i]].first>dist1[i].first) //first distance，second car_id
                        dist2[nodex_border_in_father[i]]=dist1[i];            //update the pair
                }
            }
        }

		if(y!=root) // do till arrive root  
		{
			int tot0=0,tot1=0; //dp，0begin size，tot1 end size
			for(int i=0;i<dist2.size();i++)
			{
				if(dist2[i].second==start_id){  //if StartID contained，means will influence
                    begin[tot0++]=i; //second  leaf node ID
                } else end[tot1++]=i;
			}
			int *dist;
			int n_=node[y].dist.n;

			int *nodey_dist_a=node[y].dist.a;
            for (int i = 0; i < tot0; i++) {
                dist = begin[i] * n_ + nodey_dist_a;
                for (int j = 0; j < n_; j++) {
                    buffer[j] = dist2[begin[i]].first + dist[j];
                }
                for (int j = 0; j < n_; j++) {
                    if (dist2[j].first > buffer[j]) {
                        dist2[j].first = buffer[j];
                        dist2[j].second = dist2[begin[i]].second;
                    }
                }
            }
		}
		return re;
	}

    //* del min_car_dist desde el cual node_id=start_id, luego actualice. no existe devuelve falso, de lo contrario devuelve verdadero
    bool push_borders_up_del_min_car_dist(int x,int start_id)
	{
		if(node[x].father==0)return false; //till top 
		bool re=false;
		int y=node[x].father;
		vector<pair<int,int> > &dist1=node[x].min_car_dist,&dist2=node[y].min_car_dist;
		int tot0=0,tot1=0;
		int nodex_borders_size=node[x].borders.size();
		vector<int> &nodex_border_in_father=node[x].border_in_father;
		if(y==root)//del node_id in x.father
		{
			for(int i=0;i<nodex_borders_size;i++)
			if(nodex_border_in_father[i]!=-1)
			{
				if(dist2[nodex_border_in_father[i]].second==start_id)
				{
					dist2[nodex_border_in_father[i]]=make_pair(INF,-1);
					re=true;
				}
			}
			for(int i=0;i<nodex_borders_size;i++)
			{
				if(nodex_border_in_father[i]!=-1)
				{
					if(dist2[nodex_border_in_father[i]].first>dist1[i].first)
						dist2[nodex_border_in_father[i]]=dist1[i]; 
				}
			}
		}
		else
		{
			int SIZE=node[y].borders.size();
			vector<int> &nodey_border_son_id=node[y].border_son_id;
			vector<int> &nodey_border_in_son=node[y].border_in_son;
			for(int i=0;i<SIZE;i++)
			{
				if(dist2[i].second==start_id)
				{
					dist2[i]=node[nodey_border_son_id[i]].min_car_dist[nodey_border_in_son[i]];
					end[tot1++]=i; //color END needs DP
					re=true;       //yes it is found
				}
				else begin[tot0++]=i;
			}
		}


		if(re)                //If  vertex is be delete（not root） in this node, recalculate
		{
			if(y!=root)      //root is connected all
			{
				int* dist;
				int n_=node[y].dist.n;
				int* nodey_dist_a=node[y].dist.a;
				for(int j=0;j<tot1;j++)
				{
					dist=end[j]*n_+ nodey_dist_a;
					for(int i=0;i<tot0;i++)
					{
						if(dist2[end[j]].first>dist2[begin[i]].first+dist[begin[i]])
						{
							dist2[end[j]].first=dist2[begin[i]].first+dist[begin[i]];
							dist2[end[j]].second=dist2[begin[i]].second;
						}
					}
				}
				//DP
                //first fot start, then calculte end

				dist=node[y].dist.a;
				while(tot1)
				{
					for(int i=0;i<tot1-1;i++)
						if(dist2[end[i]].first<dist2[end[i+1]].first)swap(end[i],end[i+1]); //sort from small to big
					tot1--;    
					//use end[tot1]to calculate dist2
					int i_=end[tot1];
					for(int j=0;j<tot1;j++)
					{
						if(dist2[end[j]].first>dist2[i_].first+dist[i_*n_+end[j]])
						{
							dist2[end[j]].first=dist2[i_].first+dist[i_*n_+end[j]];
							dist2[end[j]].second=dist2[i_].second;
						}
					}
				}

			}
		}
		return re;
	}

    //Calcular S a x.padre y caché
    int push_borders_up_cache_KNN_min_dist_car(int x)
    {
      //DP
      if (node[x].father == 0) return INF;
      int re = INF + 1;

      int y = node[x].father; //向上push到父图
      node[y].cache_vertex_id = node[x].cache_vertex_id; //int cache_id,向上一层子图的cache所保存的起点编号更新
      node[y].cache_bound = -1;
      vector<int>* dist1 = &node[x].cache_dist, *dist2 = &node[y].cache_dist;

      for (int i = 0; i < (*dist2).size(); i++)   (*dist2)[i] = INF; //init
      for (int i = 0; i < node[x].borders.size(); i++) {
        if (node[x].border_in_father[i] != -1)
          (*dist2)[node[x].border_in_father[i]] = (*dist1)[i];
      } //if border same，direct copy 

      Matrix& dist = node[y].dist;
      int tot0 = 0, tot1 = 0;

      for (int i = 0; i < (*dist2).size(); i++) {
        if ((*dist2)[i] < INF) begin[tot0++] = i;  //calculated
        else
          end[tot1++] = i;                         //un-calculated
      } 

      for (int i = 0; i < tot0; i++) {
        int i_ = begin[i];
        for (int j = 0; j < tot1; j++) {
          if ((*dist2)[end[j]] > (*dist2)[i_] + dist[i_][end[j]]){
              (*dist2)[end[j]] = (*dist2)[i_] + dist[i_][end[j]];
          }
        }
      }

      if (y == root) {
        re = INF + 1;
      } else {
        for (int i = 0; i < node[y].borders.size(); i++)
          if (node[y].border_in_father[i] != -1) {
            re = min(re, (*dist2)[i]);
          }
      }
      return re;
    }

    //La mayoría importa uno, calcula el KNN de S, devuelve el conjunto de resultados
	vector<int> KNN_min_dist_car(int S,int K)
	{
		//optimization as in paper, only need to calculate some layer, only K is farther than current or non car in current layer.
		int Now_Cache_Node_Number=id_in_node[S],Now_Cache_Dist=0;
		priority_queue<pair<int,pair<int,int> > >q;//save K minimal <-dist,<node_id,border_id>>
		{//构建S的cache
			node[id_in_node[S]].cache_vertex_id=S;        //cache cache_id  
			node[id_in_node[S]].cache_bound=INF;
			node[id_in_node[S]].min_border_dist=0; //the distance from S to nearest border, prepare prune
			node[id_in_node[S]].cache_dist[0]=0;   //the distance from S to all borders  
			/*for(int p=id_in_node[S];p!=root;p=node[p].father)
				push_borders_up_cache_KNN_min_dist_car(p);*/
		}
		//PQ
		for(int i=0;i<node[Now_Cache_Node_Number].borders.size();i++) 
        {
			q.push(make_pair(-(node[Now_Cache_Node_Number].cache_dist[i]+node[Now_Cache_Node_Number].min_car_dist[i].first),make_pair(Now_Cache_Node_Number,i)));
        }

		vector<int>ans,ans2;// car_id, node_id(car_id in )
		if(Distance_Offset==false) //no offset
		{
			while(K)
			{
				int Dist=q.top().first;
				int node_id=q.top().second.first;
				int border_id=q.top().second.second;
				int real_Dist=-(node[node_id].cache_dist[border_id]+node[node_id].min_car_dist[border_id].first);
                //real distance
                if (Dist != real_Dist){ //no changed, no computing
					q.pop();  
					q.push(make_pair(real_Dist,make_pair(node_id,border_id))); //put the right one 
					continue;
				}

				if(-Dist>Now_Cache_Dist && Now_Cache_Node_Number!=root)  //Now_Catch_Dist ，distance from s to up graph 
                                                                         //if the distance is bigger than Dist, no need to explore
				{
					Now_Cache_Dist=push_borders_up_cache_KNN_min_dist_car(Now_Cache_Node_Number);
					Now_Cache_Node_Number=node[Now_Cache_Node_Number].father;
					for(int i=0;i<node[Now_Cache_Node_Number].borders.size();i++)
					{
                        q.push(make_pair(-(node[Now_Cache_Node_Number].cache_dist[i]+node[Now_Cache_Node_Number].min_car_dist[i].first),make_pair(Now_Cache_Node_Number,i)));
					}
					continue;
				}
				
				int real_node_id=node[node_id].min_car_dist[border_id].second;
				if(real_node_id==-1)break;  //if not larger than k, get all 
				int car_id=car_in_node[real_node_id][0];

				q.pop();
				ans.push_back(car_id);
				ans2.push_back(real_node_id);
				del_car(real_node_id,car_id);

				q.push(make_pair(-(node[node_id].cache_dist[border_id]+node[node_id].min_car_dist[border_id].first),make_pair(node_id,border_id)));
                //put s again, for changed
				K--;
			}
			for(int i=0;i<ans.size();i++) add_car(ans2[i],ans[i]); //roll back delete
	
		}
		else  //stop 
		{
			priority_queue<int>KNN_Dist;
			vector<int>ans3;
			while(KNN_Dist.size()<K||KNN_Dist.top()>-q.top().first) 
			//the k smaller, as the smallest will bigger and bigger

			{
				int Dist=q.top().first;
				int node_id=q.top().second.first;
				int border_id=q.top().second.second;
				int real_Dist=-(node[node_id].cache_dist[border_id]+node[node_id].min_car_dist[border_id].first);
				if(Dist!=real_Dist)
				{
					q.pop();
					q.push(make_pair(real_Dist,make_pair(node_id,border_id)));
					continue;
				}
				if(-Dist>Now_Cache_Dist && Now_Cache_Node_Number!=root)
				{
					Now_Cache_Dist=push_borders_up_cache_KNN_min_dist_car(Now_Cache_Node_Number);
					Now_Cache_Node_Number=node[Now_Cache_Node_Number].father;
					for(int i=0;i<node[Now_Cache_Node_Number].borders.size();i++)
						q.push(make_pair(-(node[Now_Cache_Node_Number].cache_dist[i]+node[Now_Cache_Node_Number].min_car_dist[i].first),make_pair(Now_Cache_Node_Number,i)));
					continue;
				}
				int real_node_id=node[node_id].min_car_dist[border_id].second;
			
				if(real_node_id==-1)break;
				int car_id=car_in_node[real_node_id][0];
				q.pop();
				del_car(real_node_id,car_id);
				q.push(make_pair(-(node[node_id].cache_dist[border_id]+node[node_id].min_car_dist[border_id].first),make_pair(node_id,border_id)));
				int car_dist=get_car_offset(car_id)-real_Dist;
				if(KNN_Dist.size()<K)KNN_Dist.push(car_dist);
				else if(KNN_Dist.top()>car_dist)
				{
					KNN_Dist.pop();
					KNN_Dist.push(car_dist);
				}
				ans.push_back(car_id);
				ans2.push_back(real_node_id);
				ans3.push_back(car_dist);
			}
			for(int i=0;i<ans.size();i++)add_car(ans2[i],ans[i]);
			int j=0;
			for(int i=0;i<ans.size();i++)
				if(ans3[i]<=KNN_Dist.top())
					ans[j++]=ans[i];
			while(ans.size()>K)ans.pop_back();
		}
		return ans;
	}
}tree;

// Inciaizar usando una semilla
void init(){
	srand(747929791);
}

// Leer archivo con los datos de vertices , aristas y pesos
void read()
{
	printf("begin read\n");
	FILE *in=NULL;
	in=fopen(Edge_File,"r");
	cout<<"correct1"<<endl;
	fscanf(in,"%d %d\n",&G.n,&G.m);
	cout<<G.n<<" "<<G.m<<endl;
	cout<<"correct2"<<endl;
	G.init(G.n,G.m);
	for(int i=0;i<G.n;i++)G.id[i]=i;
	cout<<"correct3"<<endl;
	int i,j,k,l;
	for(i=0;i<G.m;i++)//Load edge
	{
		//int temp;
		fscanf(in,"%d %d %d\n",&j,&k,&l);
        if (NWFIX == 1) {
            if (RevE == false) G.add_D(j, k, l); //Undirected
            else G.add(j,k,l);//Directed
        } else {
            if (RevE == false) G.add_D(j - 1 , k - 1, l); //Undirected
            else G.add(j - 1, k - 1, l); //Directed
        }
    }
	cout<<"correct4"<<endl;
	fclose(in);
}

// Guardar en un archivo tras la construccion
void save(string file_name)
{
	printf("begin save\n");
	ofstream out(file_name.c_str());
	out<<G.n<<' ';
	out<<tree;
	out.close();
	printf("save_over\n");
}

// Guardar en un archivo binario
void save_binary(string file_name)
{
	printf("begin save_binary\n");
	ofstream out(file_name.c_str());
	save(out,G.n);
	tree.save_binary(out);
	out.close();
	printf("save_binary_over\n");
}

// Carga de archivo normal
void load(string file_name)
{
	ifstream in(file_name.c_str());
	in>>G.n;
	in>>tree;
}

// Carga de archivo binario
void load_binary(string file_name)
{
	printf("begin load_binary\n");
	ifstream in(file_name.c_str());
	read(in,G.n);
	printf("load_binary G.n=%d\n",G.n);
	tree.read_binary(in);
	in.close();
	printf("load_binary_over\n");
}

//* Algunos datos especiales NW,US desde 0
bool arg_has_str(char ar[], string query_str) {
    string strtemp = string(ar);
    if (strtemp.find(query_str) == std::string::npos) {
        return false;
    }
    return true;
}

// Funcion para testear consulta knn
void knn_test(double car_percent, double change_percent, int query_number, int K, int query_time) {
            vector<int> ans;
            vector<int> addr;
            vector<int> query_pos;
            addr.clear();

       int car_num = G.n * car_percent;
       int change_num = int (car_num * change_percent); 
       int sun_change_num=0;
        printf("KNN Test: car_percent=%lf change_percent=%lf K=%d\n",car_percent,change_percent,K);
        printf("Total Vertice Number is %d\n",G.n);
        printf("Total Car Number is %d\n",car_num);
        printf("Change number of vehicles per query: %d\n",query_number);
        printf("Query number after each update: %d\n",change_num);
        printf("Every test repeat %d times.\n",query_time);
        
            for (int i = 0; i < car_num; i++) {      //generate position of car
                int S = rand() % G.n;
                addr.push_back(S);
            }

            for (int i = 0; i < query_time; i++) {   //generate query position
                int te_query = rand() % G.n;
                query_pos.push_back(te_query);
            }

            for (int i = 0; i < car_num; i++) { //add cars to road network
                tree.add_car(addr[i], i);
            }

            TIME_TICK_START();     //one time knn
            for (int i = 0; i < query_time; i++) {
                ans = tree.KNN_min_dist_car(query_pos[i], K);
            }
            TIME_TICK_END();

            TIME_TICK_PRINT("ONE time knn",query_time);


                //mutiple test
            long long total_up_time  = 0;
            long long total_query_time  = 0;
            long long sin_up_time  = 0;
            long long sin_que_time  = 0;
            
            for (int t = 0; t < query_time; t++){

            	TIME_TICK_START();
	            for (int i = 0; i < change_num; i++) {  //Change the position of vehicle
	                int change_car = rand() % car_num;
	                int old_pos = addr[change_car];
	                int new_pos = (addr[change_car] + 1) % G.n;
	                addr[change_car] = new_pos;
	                tree.del_car(old_pos, change_car);
	                tree.add_car(addr[change_car], change_car);
	            }
		   		TIME_TICK_END();

		   		sin_up_time = te - ts;
		   		total_up_time += sin_up_time ;
		   		//printf("%lld sing_uptime %lld total_up_time", sin_up_time, total_up_time);
		   		//TIME_TICK_PRINT("updating cost",query_time);

		   		TIME_TICK_START();
	            for (int i = 0; i < query_number; i++) {
	                ans = tree.KNN_min_dist_car(query_pos[i], K);
	            }
	            TIME_TICK_END();
	            sin_que_time = te - ts;
	            total_query_time += sin_que_time;
	        }

	        long long ave_amor_time = (total_query_time + total_up_time) / (query_number * query_time);
	        long long ave_query_time  = total_query_time / (query_time * query_number);
	        printf ("Query Time \t %lld (us)\t Amortized Time \t %lld (us)", ave_query_time, ave_amor_time);
           	fflush(stdout);

            //TIME_TICK_PRINT("Mutiple Time knn",query_time);
}
// *********************************************
// * MAIN
// *********************************************
int main(int argc, char *argv[])
{
    	string loadfile;
        if(argc<6)
        {
            printf("Load VTree and run KNN test use './vtree_knn_demo  VTree_file $K(int) $car_percent(int) $change_percent(int) $query_per_update'\n");
            return 0;
        }
        loadfile= argv[1];
        int K = std::atoi(argv[2]);
        float car_per = std::atof(argv[3]);
        float change_per = std::atof(argv[4]);
        float query_number = std::atof(argv[5]);
    	fflush(stdout);
    	TIME_TICK_START();
    	init();
        load_binary(loadfile);
        cout << "***************************************" << endl;
        cout << "Finish Load File " << loadfile << "vertice Number:\t"<< G.n<< endl;
        cout << "***************************************" << endl << endl;
    	fflush(stdout);
        TIME_TICK_END();

    	knn_test(car_per, change_per,query_number, K,10000);
    	printf("Program End.\n");fflush(stdout);
        return 0;
}