# Chianina (also named as GraphFlow)

Welcome to the home repository of Chianina.

Chianina --- A Disk-based Envolving Graph System for Context- and Flow-Sensitive Static Analyses on Million Lines of C Code.

This README provides a how-to-use guide for Chianina. 

## Getting started

### Required Libraries

Make sure that you have the Boost library installed in your environment. Here you can get started using [Boost](https://www.boost.org/doc/libs/1_69_0/more/getting_started/index.html).

### Compiling Chianina

1. Download the entire Chianina source code into your machine.

```bash
$ git clone https://github.com/Chianina-system/backend.git
```

2. Get into the folder where contains the makefile.

```bash
$ cd backend/Debug/
```

3. Run makefile

```bash
$ make
```

After compiling, there should be a `backend` binary executable file in `Debug` folder.

### Running Chianina

Chianina needs five input files:

1. **file_total**: record total number of nodes in the cfg 

2. **file_entries**: record entry nodes' id

3. **file_cfg**: record the whole program(inter) cfg which Chianina can perform computation

4. **file_stmts**: record a map from node id to statement

5. **file_singletons**: record singleton variable ids

And there are also seven parameters used to fit your own program and running environment: 

1. **graphstore_mode**: `0 -- naive, 1 -- itemset`

2. **update_mode**: `0 -- sequential, 1 -- parallel`

3. **comp_mode**: `0 -- in-memory, 1 -- out-of-core`

4. **num_partitions**: the number of partitions you want to part the in the computation cfg (if comp_mode == 1)

5. **file_mode**: `0 -- binary, 1 -- text` (if comp_mode == 1)

6. **buffered_mode**: `0 --default, 1 -- specified` (if comp_mode == 1)

7. **mining_mode**: `0 -- eclat, 1 -- apriori, 2 -- fg-growth` (if comp_mode == 1)

```bash
./backend <file_total> <file_entries> <file_cfg> <file_stmts> <file_singletons> <graphstore_mode> <update_mode> <comp_mode> <num_partitions> <file_mode> <buffered_mode> <mining_mode>
```

#### Usage example

```bash
./backend configuration.txt entry.txt final id_stmt_info.txt var_singleton_info.txt 0 1 1 1 0 1 0
```





### Project Contributors
* [**Zhiqiang Zuo**](http://z-zhiqiang.github.io/) - *Assistant Professor, Nanjing University*
* **Yiyu Zhang** - *PhD Student, Nanjing University*
* **Zewen Sun** - *PhD Student, Nanjing University*
* **Duanchen Xu** - *Master Student, Nanjing University*
