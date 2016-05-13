#include "RStarTree.h"
#include "TimerTool.h"
#include "ConfigTool.h"

void TraverseTree(RSTNode<TreeDataR, double> *node, int depth)
{
	if(node->is_leaf_node()) //leaf node
	{
		RSTLeafNode<TreeDataR, double> *leaf=static_cast<RSTLeafNode<TreeDataR, double> *>(node);
		
		//print this node
		double *mbr=new double[2*leaf->dim];
		leaf->get_mbr(mbr);
		for(int j=0;j<depth;j++) printf("    ");
		printf("Leaf id: %d mbr: ", leaf->page_id);
		for(int j=0;j<2*leaf->dim;j++) printf("%f ", mbr[j]);
		printf("\n");
		
		//print data
		for(int i=0;i<leaf->entry_num;i++)
		{
			TreeDataR<double> *mbr_data=leaf->data[i];
			for(int j=0;j<=depth;j++) printf("    ");
			printf("data id: %d mbr: ", mbr_data->id);
			for(int j=0;j<2*leaf->dim;j++) printf("%f ", mbr_data->data[j]);
			printf("\n");
		}

		delete[] mbr;
	}
	else //nonleaf node
	{
		RSTNonLeafNode<TreeDataR, double> *nonleaf=static_cast<RSTNonLeafNode<TreeDataR, double> *>(node);

		//print this node
		double *mbr=new double[2*nonleaf->dim];
		nonleaf->get_mbr(mbr);
		for(int j=0;j<depth;j++) printf("    ");
		printf("Non Leaf id: %d mbr: ", nonleaf->page_id);
		for(int j=0;j<2*nonleaf->dim;j++) printf("%f ", mbr[j]);
		printf("\n");

		for(int i=0;i<nonleaf->entry_num;i++)
		{
			TraverseTree(nonleaf->get_child(i), depth+1);
		}

		delete[] mbr;
	}
}

void TraverseTree(RSTNode<TreeDataP, double> *node, int depth)
{
	if(node->is_leaf_node()) //leaf node
	{
		RSTLeafNode<TreeDataP, double> *leaf=static_cast<RSTLeafNode<TreeDataP, double> *>(node);
		
		//print this node
		double *mbr=new double[2*leaf->dim];
		leaf->get_mbr(mbr);
		for(int j=0;j<depth;j++) printf("    ");
		printf("Leaf id: %d mbr: ", leaf->page_id);
		for(int j=0;j<2*leaf->dim;j++) printf("%f ", mbr[j]);
		printf("\n");
		
		//print data
		for(int i=0;i<leaf->entry_num;i++)
		{
			TreeDataP<double> *mbr_data=leaf->data[i];
			for(int j=0;j<=depth;j++) printf("    ");
			printf("data id: %d point: ", mbr_data->id);
			for(int j=0;j<leaf->dim;j++) printf("%f ", mbr_data->data[j]);
			printf("\n");
		}

		delete[] mbr;
	}
	else //nonleaf node
	{
		RSTNonLeafNode<TreeDataP, double> *nonleaf=static_cast<RSTNonLeafNode<TreeDataP, double> *>(node);

		//print this node
		double *mbr=new double[2*nonleaf->dim];
		nonleaf->get_mbr(mbr);
		for(int j=0;j<depth;j++) printf("    ");
		printf("Non Leaf id: %d mbr: ", nonleaf->page_id);
		for(int j=0;j<2*nonleaf->dim;j++) printf("%f ", mbr[j]);
		printf("\n");

		for(int i=0;i<nonleaf->entry_num;i++)
		{
			TraverseTree(nonleaf->get_child(i), depth+1);
		}

		delete[] mbr;
	}
}

TreeDataP<double> **generate_data(int data_num, int dim)
{
	TreeDataP<double> **data=new (TreeDataP<double>* [data_num]);
	for(int i=0;i<data_num;i++)
	{
		data[i]=new TreeDataP<double>(dim);

		data[i]->id=i;
		for(int j=0;j<dim;j++) data[i]->data[j]=getrand(); //get data randomly
	}

	return data;
}

void release_data(TreeDataP<double> **data, int data_num)
{
	for(int i=0;i<data_num;i++) delete data[i];
	delete[] data;
}

void load_exsiting_rtree_p(char *file_name)
{
	//read exsisting rtree
	//dimension and pagesize is stored, we can get it from tree after reading the file
	bool force_new=false; //do not overwrite
	RStarTree<TreeDataP, double> *tree=new RStarTree<TreeDataP, double>(file_name, 0, 0, ".",
		PageFile::C_FULLMEM, 0, force_new);

	tree->load_root();
	TraverseTree(tree->root, 0);
	
	delete tree;
}

void test_insert(char *file_name, int dim, int page_len, TreeDataP<double> **data, int data_num)
{
	//the memory policy of page file
	PageFile::c_policy pols[]={PageFile::C_FULLMEM, PageFile::C_LRU, PageFile::C_MRU, PageFile::C_NO_CACHE};
	int pagefile_cache_size=0; //we use full memory
	bool force_new=true; //if file exists, we will force overwrite it

	RStarTree<TreeDataP, double> *tree=new RStarTree<TreeDataP, double>(file_name, dim, page_len, ".",
		pols[0], pagefile_cache_size, force_new);
	
	double t=-gettime();

	//construct tree using insertion
	for(int i=0;i<data_num;i++)
	{
		tree->insert(data[i]);
	}

	t+=gettime();
	printf("\nInsertion using %f sec.\n", t);

	//flush all things to memory
	delete tree;

	//load_exsiting_rtree_p(file_name);
}

void test_bulkload(char *file_name, int dim, int page_len, TreeDataP<double> **data, int data_num)
{
	//the memory policy of page file
	PageFile::c_policy pols[]={PageFile::C_FULLMEM, PageFile::C_LRU, PageFile::C_MRU, PageFile::C_NO_CACHE};
	int pagefile_cache_size=0; //we use full memory
	bool force_new=true; //if file exists, we will force overwrite it

	//bulkload
	RStarTree<TreeDataP, double> *tree=new RStarTree<TreeDataP, double>(file_name, dim, page_len, ".",
		pols[0], pagefile_cache_size, force_new);
	
	double t=-gettime();

	//construct tree using STR bulkloading
	tree->bulkload_str(data, data_num, 0.7);

	t+=gettime();
	printf("\nBulkloading using %f sec.\n", t);

	//flush all things to memory
	delete tree;

	//load_exsiting_rtree_p(file_name);
}



void test(int dim, int page_len, int data_num)
{
	TreeDataP<double> **data=generate_data(data_num, dim);

	test_insert("insert.rt", dim, page_len, data, data_num);

	test_bulkload("bulkload.rt", dim, page_len, data, data_num);
	
	release_data(data, data_num);
}

int main(int argc, char **argv)
{
	setseed((unsigned long)time(NULL));

	ConfigTool cfg;
	cfg.AddConfigFromCommandLine(argc, argv);
	if(GenericTool::CheckPathExistence("./config.ini")) cfg.AddConfigFromFile("./config.ini");
	if(!cfg.IfExist("dim")) cfg.ModifyConfig("dim", "2");
	if(!cfg.IfExist("page_len")) cfg.ModifyConfig("page_len", "4096");
	if(!cfg.IfExist("data_num")) cfg.ModifyConfig("data_num", "50000");
	cfg.ListConfig();

	int dim=cfg.GetConfigInt("dim");
	int page_len=cfg.GetConfigInt("page_len");;
	int data_num=cfg.GetConfigInt("data_num");;

	test(dim, page_len, data_num);
	
	return 0;
}