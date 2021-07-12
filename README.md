# Chianina interface 

Welcome to the GraphFlow analysis tool based on the Chinaina interface implementation.

This document is an extended description of Chianina system. It introduces the framework  implementation of Chianina, A disk-based Envolving Graph System, and two different analysis application based on this framework, alias analysis and cache analysis.

This guide provides the illustraction of framework interface and how to use the two analysis applications. 

## Getting started
###  Compiling and Running Alias Analysis
1. Download the entire Chianina source code into your machine.

```bash
$ git clone https://github.com/Chianina-system/backend-private.git
```

2. Get into the folder where contains the makefile.

```bash
$ cd backend-private-alias_impl/Debug/
```

3. Run makefile

```bash
$ make
```

After compiling, there should be a `backend` binary executable file in `Debug` folder.

#### Usage example
```bash
./backend configuration.txt entry.txt final id_stmt_info.txt var_singleton_info.txt 0 1 1 1 0 1 0
```

###  Compiling and Running Cache Analysis
1. Download the entire Chianina source code into your machine.

```bash
$ git clone https://github.com/Chianina-system/backend-private.git
```

2. Get into the folder where contains the makefile.

```bash
$ cd backend-private-cache_impl
```

3. Check CMakelists.txt and Run makefile

```bash
$ cmake CMakelists.txt
$ make
```

After compiling, there should be a `backend_tuning` binary executable file in `cache_analysis` folder.


#### Usage example

```bash
./backend_tuning configuration.txt entry.txt new-final.txt new-nodes.txt id_stmt_info.txt 0 1 1 1 0 1 0 0 0
```

### Interface Illustration

#### Data Structure
1 . cfg_node.h

Class `CFGNode`,  stores program CFG node information, composed of stmts.

The virtual functions ` size_t get_size_bytes()`,  ` size_t get_size_bytes()`,  ` size_t get_size_bytes()`, define the size of the buffer used fby IO. Thus, the memory or disk space graph file occupied can be estimated to help serialization and deserialization operation.

```c++
virtual size_t get_size_bytes() const = 0;
virtual void write_to_buf(Buffer& buf) = 0;
virtual void read_from_buf(char* buf, size_t bufsize) = 0;
virtual inline Stmt* getStmt() const  = 0;

PEGraph_Pointer getOutPointer() const；
void setCfgNodeId(int cfgNodeId);
```

Class `CFG_node_alias` is the implementation  of alias analysis node based on this interface. Each node is a statement with a corresponding semantic type.

Class `CFG_node_cache` is a cache analysis node, similarly. Each node contains a set of instructions.



2 . cfg.h

Class `CFG`, stores node and edge information of program CFG, as well as the predesessor and successor nodes of each node.

In CFG, the number of nodes and edges, and the relationship between nodes is not related to node type, so a general interface can be extracted, as shown below.

`addOnePred` and `addOneSucc`,  find one pred or succ node according to the edge.

`getPredesessors`和`getSuccessors`,   obtain pred or succ nodes for specified node.

`getNodes`和`getNumberEdges`,  obtain total number of nodes an edges in CFG.

```c++
void addOnePred(CFGNode *succ, CFGNode *pred); //
void addOneSucc(CFGNode *pred, CFGNode *succ);
void addOneNode(CFGNode *Node);
std::vector<CFGNode*>* getPredesessors(const CFGNode* node);
std::vector<CFGNode*>* getSuccessors(const CFGNode* node);

std::vector<CFGNode*>& getNodes();
vertexid_t getNumberEdges() const;
```



3 . cfg_map.h

The class `CFG_map` inherits from class `CFG`. It describes how graph information is loaded from memory. In contrast to class `CFG`, the following implementation is added.

Among them, `loadCFG` is a virtual function, which abstractly expresses the graph file information required by different GraphFlow applications for calculation.

```c++
std::vector<CFGNode*> getEntryNodes() const;

virtual void loadCFG(const string& file_cfg, const string& file_stmt, const string& file_entries) = 0;
```

Class `CFG_map_alias` is the memory version of alias analysis implemented based on this interface.

Class `CFG_map_cache` is similar. The load of alias flow and cache flow both depend on the specific node type.





4 . cfg_map_outcore.h

The class `CFG_map_outcore` also inherits from class `CFG`. It describes how to load graph information from different partitions on disk when performing graph flow calculations outcore. Compared to the class `CFG_map`, there are differences as following:

Among them,`loadCFG_ooc` is a virtual function,similar to `loadCFG`, which abstractly expresses the needed graph file information and the way to load it when computing different graph flows in outcore.

As is assumed that edges do not span partitions, nodes may be in more than one partition at the same time, so it is called mirror node. You can use `isMirror` function to know whether a node crosses partition.

```c++
bool isMirror(CFGNode* node);
std::unordered_set<CFGNode*>& getActiveNodes();

virtual void loadCFG_ooc(bool file_mode, const string& file_cfg, const string& file_stmt, const string& file_mirrors_call, const string& file_mirrors_shallow, const string& file_actives) = 0;
```

Based on this interface, class `CFG_map_outcore_alias` implements alias Flow load from outcore.



5 . graphstore

It describes the graph state information stored in the calculation process of GraphFlow, as well as the serialization and deserialization operations of read and write when the nodes perform IO reads and writes.

```c++
virtual void loadGraphStore(const string& file, const string& file_in, Partition part, int mining_mode, int support, int length) = 0;
virtual void deserialize(const string& file) = 0;
virtual void serialize(const string& file) = 0;
virtual void store_in_graphs(const string& file_graphs_in, std::unordered_set<CFGNode*>& set) = 0;
```

The extended `graphstore_alias` realizes the retrieval and updating mode of graph flow based on this interface.

```c++
virtual PEGraph* retrieve(PEGraph_Pointer graph_pointer) = 0;
virtual void update_graphs_sequential(GraphStore_alias* another) = 0;
virtual void update_graphs_parallel(GraphStore_alias* another) = 0;
```

The extended `graphstore_cache` implements these functions. It is obvious that the retrieved graphflow storage table has a different return type to `GraphStore_Cache`.

```c++
virtual cachestate* retrieve(PEGraph_Pointer graph_pointer) = 0;
virtual std::unordered_map<PEGraph_Pointer, cachestate*>& getMap() = 0;
```



#### Compute Mode
1 . cfg_compute_syn.h

The class `cfg_compute_syn.h`,  abstract the core calculation steps of graph flow in memory:

`load`, describe the process of loading graph information needed to compute graph flow  from memory。

`do_worklist_synchronous`，the implementation of worklist algorithm which calculating and transferring graph flow。

`transfer`， the implementation of node calculating graph flow in the worklist algorithm.

`propagate`，the implementation of graph flow passing information to successor nodes in the worklist algorithm.

```c++
static bool load(const string& file_total, const string& file_cfg, const string& file_stmt, const string& file_entries, CFG *cfg_, const string& file_singleton, Singletons* singletons, GraphStore *graphstore, const string& file_grammar, Grammar * grammar);

static void do_worklist_synchronous(CFG* cfg, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode);

static PEGraph* transfer(PEGraph* in, Stmt* stmt, Grammar* grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer = nullptr);

static void propagate(CFGNode *cfg_node, CFG *cfg, PEGraph *out, Grammar *grammar, Concurrent_Worklist<CFGNode*> *worklist_2);
```

The interface `cfg_compute_syn_alias.h`,   while inheriting the above interface, it also implements the transfer process corresponding to different statement types. 

And it abstracts the common computing procedure of  three different analysis modes，which means the unique worklist algorithm of alias analysis.

```c++
static PEGraph* transfer_address(PEGraph* in, AllocStmt* stmt,Grammar* grammar, Singletons* singletons, bool flag, Timer_wrapper_inmemory* timer);

static void do_worklist_synchronous(CFG* cfg_, DeltaGraphStore_alias* graphstore, Grammar* grammar, Singletons* singletons, bool flag, bool update_mode);
```

The interface `cfg_compute_syn_cache.h` is similar to the alias analysis.



2 . cfg_compute_ooc_syn.h

`cfg_compute_ooc_syn.h`,  describes the process of computing graphflow in outcore. Partitioning, graph load and serialization need to be considered, so the following functions are also implemented:

`pass`,  graph flow is passed between different partitions.

`store_in_actives`,  active node set for next iteration.

```c++
static bool load(Partition part, CFG *cfg_, GraphStore *graphstore, Context *context, bool file_mode, int mining_mode, int support, int length);

static void do_worklist_ooc_synchronous(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag, bool update_mode, Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer);

static void pass(Partition partition, CFG *cfg, GraphStore *graphstore, Concurrent_Worklist<CFGNode *> *actives, Context *context, bool file_mode);

static void store_actives(bool file_mode, const string& file_actives, std::unordered_set<CFGNode*>& set);

static void store_in_graphs(const string &file_graphs_in, CFG *cfg, GraphStore *graphstore, std::unordered_set<CFGNode *> &set);
```









