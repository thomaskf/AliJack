#include<vector>
#include<string>
#include<fstream>
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<ctime>

#define VERSION 1.1

using namespace std;

typedef vector<int> intarray;

string i2c[] = {"0","1","2","3","4","5","6","7","8","9"};

class options {
public:
	string infilename;
	string outprefix;
	int k;
	int n;
	string grpfilename;
	int seed;
	int relabelSeq;
	options() {
		// default values
		infilename = "";
		outprefix = "";
		k = -1;
		n = -1;
		grpfilename = "";
		seed = -1;
		relabelSeq = 0;
	}
};

string int2str(int x) {
	// assume: x >= 0
	if (x < 10)
		return i2c[x];
	else
		return int2str(x/10) + i2c[x%10];
}

void removeSpaces(string& s) {
	// remove all spaces inside the string
	int i,k;
	k=0;
	for (i=0; i<s.length(); i++) {
		if (s[i]!=' ') {
			if (k<i)
				s[k]=s[i];
			k++;
		}
	}
	if (k==0)
		s = "";
	else if (k<s.length())
		s.resize(k);
}

void tokenizer(string seq, string separators, vector<string>& result) {
    // split the seq into many parts by "separators"
    // the vector<string> *result cannot be NULL
    result.clear();
    int startpos = (int) seq.find_first_not_of(separators);
    while (startpos != (int) string::npos) {
        int endpos = (int) seq.find_first_of(separators, startpos);
        if (endpos != (int) string::npos) {
            result.push_back(seq.substr(startpos, endpos-startpos));
            startpos = (int) seq.find_first_not_of(separators, endpos);
        } else {
            result.push_back(seq.substr(startpos));
            break;
        }
    }
}

void showSyntex(string prog) {
	cout << "Version " << VERSION << endl;
	cout << "Syntax: " << prog << " [options]" << endl << endl;
	
	cout << "Compulsory options: " << endl;
	cout << "      -i <input file> : the input alignment in fasta format" << endl;
	cout << "      -o <out prefix> : the prefix of the output alignment files" << endl;
	cout << "      -k <integer>    : # of seqs randomly selected (if -g is not used), OR" << endl;
	cout << "                        # of seqs randomly selected for each group (otherwise)" << endl;
	cout << "      -n <integer>    : # of sets of output alignments" << endl << endl;
	 
	cout << "Optional options:" << endl;
	cout << "      -g <group file> : a text file showing which sequences in the same group" << endl;
	cout << "                        format: i1  i2  i3  ...  # group 1 name" << endl;
	cout << "                                i1' i2' i3' ...  # group 2 name" << endl;
	cout << "                        (i1, i2, i3, i1',i2',i3' are integers >= 1" << endl;
	cout << "                         showing which sequences belong to the group)" << endl;
	cout << "      -s <seed number>: if not used, the seed will be initialized by time" << endl;
	cout << "      -l < 0 or 1 >   : relabel the sequence names as:" << endl;
	cout << "                                [grp1 name]_1, [grp1 name]_2, ..." << endl;
	cout << "                                [grp2 name]_1, [grp2 name]_2, ..." << endl;
	cout << "                        (by default, this option is 0 - disabled)" << endl;
	cout << "      -h              : this help menu" << endl << endl;
	
	cout << "Output files:" << endl;
	cout << "      <out prefix>.1.fa, <out prefix>.2.fa, ... , <out prefix>.<n>.fa" << endl << endl;
	
	cout << "Example files" << endl;
	cout << "1. Data file : data.fasta" << endl;
	cout << "2. Group file: data.grp.txt" << endl;
	cout << "Example command: " << prog << " -i data.fasta -g data.grp.txt -o out -k 2 -n 10" << endl;
	cout << "(Randomly select two sequences from each group, and produce 10 sets of data)" << endl;
	cout << "Example output files: out.1.fa, out.2.fa, ..., out.10.fa" << endl << endl;
	
	cout << "Contact: Lars Jermiin <Lars.Jermiin@csiro.au>" << endl;
	cout << "         Thomas Wong <Thomas.Wong@anu.edu.au>" << endl;
	exit(1);
}

void showWelcomeMsg(string& prog, options& usr_option) {
	// show welcome message and list out the options
	cout << "=======================================" << endl;
	cout << "　　Welcome to AliJack version " << VERSION << endl;
	cout << "=======================================" << endl << endl;
	cout << "Input file      : " << usr_option.infilename << endl;
	cout << "Out file prefix : " << usr_option.outprefix << endl;
	cout << "Value of k      : " << usr_option.k << endl;
	cout << "Value of n      : " << usr_option.n << endl;
	if (usr_option.grpfilename != "")
		cout << "Group file      : " << usr_option.grpfilename << endl;
	cout << "Relabel seqs    : ";
	if (usr_option.relabelSeq == 1)
		cout << "yes" << endl;
	else
		cout << "no" << endl;
	cout << "Seed number     : " << usr_option.seed << endl << endl;
}

void readOptions(int argc, char** argv, options& usr_option) {
	int i;
	string s, v;
	char u;
	string err_message = "";
	for (i=1; i<argc; i++) {
		if (err_message != "")
			break;
		s = argv[i];
		if (s.length() >= 2 && s[0]=='-') {
			u = s[1]; // the option
			if (s.length() > 2) {
				v = s.substr(2);
			} else if (++i<argc) {
				v = argv[i];
			}
			switch(u) {
				case 'i':
					usr_option.infilename = v;
					break;
				case 'o':
					usr_option.outprefix = v;
					break;
				case 'k':
					usr_option.k = atoi(v.c_str());
					break;
				case 'n':
					usr_option.n = atoi(v.c_str());
					break;
				case 'g':
					usr_option.grpfilename = v;
					break;
				case 's':
					usr_option.seed = atoi(v.c_str());
					break;
				case 'l':
					usr_option.relabelSeq = atoi(v.c_str());
					break;
				case 'h':
					showSyntex(argv[0]);
					break;
				default : 
					err_message = "Error! Unknown option: " + s;
					break;
			}
		} else {
			err_message = "Error! Unknown option: " + s;
		}
	}
	if (err_message == "") {
		// check whether all the compulsory options have been inputted
		if (usr_option.infilename == "")
			err_message = "Error! Option -i is missing";
		else if (usr_option.outprefix == "")
			err_message = "Error! Option -o is missing";
		else if (usr_option.k == -1)
			err_message = "Error! Option -k is missing";
		else if (usr_option.n == -1)
			err_message = "Error! Option -n is missing";
		else if (usr_option.relabelSeq == 1 && usr_option.grpfilename == "")
			err_message = "Error! The group file has to be provided when the option -l is used";
		// check whether k and n are > 0
		else if (usr_option.k <= 0)
			err_message = "Error! The value of k <= 0";
		else if (usr_option.n <= 0)
			err_message = "Error! The value of n <= 0";
		// check whether relabelSeq = 0 or 1
		else if (usr_option.relabelSeq < 0 || usr_option.relabelSeq > 1)
			err_message = "Error! The value for the option -l should be 0 or 1";
	}
	if (err_message != "") {
		cout << err_message << endl;
		exit(1);
	}
}

void readFasta(string faFile, vector<string>& seqNames, vector<string>& seqs) {
	// read the fasta file
	ifstream fin;
	fin.open(faFile.c_str());
	string aline;
	while (getline(fin, aline)) {
		if (aline.length() > 0) {
			if (aline[0] == '>') {
				seqNames.push_back(aline.substr(1));
				seqs.push_back("");
			} else {
				seqs[seqs.size()-1].append(aline);
			}
		}
	}
	fin.close();
}

void loadGrpFile(vector<intarray>& grps, vector<string>& grpNames, string fileName, int totSeqNum) {
	// load the group information
	ifstream fin;
	string aline;
	vector<string> token;
	intarray isExist;
	int pos;
	int i,k;
	int linenum;
	string grpName;
	if (fileName.length() > 0) {
		for (i=0; i<totSeqNum; i++)
			isExist.push_back(0);
		fin.open(fileName.c_str());
		if (!fin.is_open()) {
			cout << "Error in opening the file " << fileName << endl;
			exit(1);
		}
		linenum = 0;
		while (getline(fin, aline)) {
			linenum++;
			if (aline.length() > 0) {
				// everything after '#' is the name of the group
				grpName = "";
				pos = aline.find_first_of('#', 0);
				if (pos == 0) {
					aline = "";
				} else if (pos < aline.length()) {
					if (pos < aline.length()-1)
						grpName = aline.substr(pos+1);
					aline = aline.substr(0, pos);
				}
				removeSpaces(grpName);
				if (grpName == "")
					grpName = "Grp" + int2str(linenum);
				// cout << "grpName=" << grpName << endl;
				grpNames.push_back(grpName);
				// collect all the integers
				tokenizer(aline, " ", token);
				grps.resize(grps.size()+1);
				for (i=0; i<token.size(); i++) {
					if (token[i].length() > 0) {
						// cout << "token[" << i << "]=" << token[i] << endl;
						k = atoi(token[i].c_str());
						if (k > totSeqNum || k < 1) {
							cout << "Error on loading the group file - " << fileName << endl;
							cout << "On " << linenum << "-th line: '" << token[i] << "' is not a valid value" << endl;
							exit(1);
						}
						k--;
						if (isExist[k]) {
							cout << "Error on loading the group file - " << fileName << endl;
							cout << "On " << linenum << "-th line: '" << token[i] << "' has appeared before" << endl;
							exit(1);
						}
						isExist[k] = 1;
						grps[grps.size()-1].push_back(k);
					}
				}
			}
		}
		fin.close();
		// check whether all the numbers have appeared in the group file
		for (i=0; i<totSeqNum; i++) {
			if (!isExist[i]) {
				cout << "Error! '" << i+1 << "' does not appear in the group file - " << fileName << endl;
				exit(1);
			}
		}
	} else {
		grps.resize(1);
		for (i=0; i<totSeqNum; i++)
			grps[0].push_back(i);
	}
}

void reorder_ids(intarray& ids, int k, int group_id) {
	// input: a set of ids of a particular group, and k
	// output: randomly pick k items and place them in the front of the array
	// assume: k <= id.size()
	
	int i,j,t,e;
	
	if (k > ids.size()) {
		cout << "Error! The value of k, which is " << k << ", is greater than the number of items in the group " << group_id << endl;
		exit(1);
	}
	
	if (k == ids.size()) // all items in the group are selected, thus no need to randomly pick k items
		return;
	
	int ignore_k = ids.size() - k; // number of items are ignored
	
	if (k <= ignore_k) {
		// randomly pick k items and place them in the front of the array
		for (i=0; i<k; i++) {
			j = (rand() % (ids.size() - i)) + i;
			// swapping between ids[i] and ids[j]
			if (i != j) {
				t=ids[i]; ids[i]=ids[j]; ids[j]=t;
			}
		}
	} else {
		// randomly pick ignore_k items and place them at the end of the array
		e = ids.size();
		for (i=0; i<ignore_k; i++) {
			j = rand() % (ids.size() - i);
			e--;
			// swapping between ids[e] and ids[j]
			if (e != j) {
				t=ids[e]; ids[e]=ids[j]; ids[j]=t;
			}
		}
	}
}

void reorder_grpids(vector<intarray>& grpids, int k) {
	// for each group, randomly pick k items and place them into the front positions of the group
	int i;
	for (i=0; i<grpids.size(); i++) {
		reorder_ids(grpids[i], k, i+1);
	}
}

void out_fa_seq(ofstream& fout, string& seqName, string& seq) {
	fout << ">" << seqName << endl;
	int i,j;
	for (i=0; i<seq.size(); i+=60) {
		j=60;
		if (j>seq.size()-i)
			j=seq.size()-i;
		fout << seq.substr(i,j) << endl;
	}
}

void output_seqs(vector<intarray>& grpids, vector<string>& grpNames, int k, int relabelSeq, string outfile, vector<string>& seqNames, vector<string>& seqs) {
	ofstream fout;
	fout.open(outfile.c_str());
	int i,j,s;
	string seqName;
	for (i=0; i<grpids.size(); i++) {
		for (j=0; j<k && j<grpids[i].size(); j++) {
			s = grpids[i].at(j);
			if (relabelSeq) {
				seqName = grpNames[i] + "_" + int2str(j+1);
			} else {
				seqName = seqNames[s];
			}
			out_fa_seq(fout, seqName, seqs[s]);
		}
	}
	fout.close();
}

int main(int argc, char** argv) {
	
	if (argc <= 1) {
		showSyntex(argv[0]);
	}
	
	// variables
	options usr_option;
	string prog;
	vector<string> seqNames;
	vector<string> seqs;
	vector<intarray> groupIDs;
	vector<string> groupNames;
	int i;
	string outFile;
	
	// read the user options
	readOptions(argc, argv, usr_option);
	
	// check the seed
	if (usr_option.seed == -1) {
		usr_option.seed = time(NULL);
	}
	
	// Program name
	prog = argv[0];
	if (prog[0] == '.' && prog.length() > 2)
		prog = prog.substr(2);
	
	// show welcome message and display the option the user inputted
	showWelcomeMsg(prog, usr_option);
	
	// initialize the seed
	srand(usr_option.seed);
	
	// read the fasta file
	readFasta(usr_option.infilename, seqNames, seqs);
	
	// load the group file
	loadGrpFile(groupIDs, groupNames, usr_option.grpfilename, seqs.size());
	
	// check whether the value of k <= the number of items in all the groups
	for (i=0; i<groupIDs.size(); i++) {
		if (usr_option.k > groupIDs[i].size()) {
			cout << "Error! The number of items in " << i+1 << "-th group is less than the value of k, which is " << usr_option.k << endl;
			exit(1);
		}
	}
	
	cout << "Output file(s)  : ";
	for (i=0; i<usr_option.n; i++) {
		// for each group, randomly pick k items and place them into the front positions of the group
		reorder_grpids(groupIDs, usr_option.k);

		// output file name
		outFile = usr_option.outprefix + "." + int2str(i+1) + ".fa";
		if (i > 0)
			cout << ", ";
		cout << outFile;
		
		// output the first k sequences of each group
		output_seqs(groupIDs, groupNames, usr_option.k, usr_option.relabelSeq, outFile, seqNames, seqs);
	}
	cout << endl;
}
