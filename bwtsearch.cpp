//============================================================================
// Name        : bwtsearch.cpp
// Author      : Jiarui Ding (z5045636)
// Version     :
// Copyright   : Your copyright notice
// Description : The following code is written for COMP9319 Assignment 2
//============================================================================

#include <iostream>
#include <fstream>
# include <stdio.h>
# include <stdlib.h>
#include <list>

using namespace std;


// Variables and Declarations :

unsigned int C_list[128];  // C List: from 0 to 127

// Each sub_index is the counts of 128(actually just 98) Ascii codes

// Adjust the below boundary:
// to get the best boundary whether to write an external index or not
int sub_idx_size = 6666;



// Get the occurrence of c in the overhead(not recorded by index)
int occ1(ifstream& bin, int pos, char c){

	int count_c = 0;

	// get to the beginning of the file
	bin.seekg(ios::beg);

	char buffer [pos+1];
	bin.read(buffer, pos+1);

	for (int x = 0; x < pos+1; x++) {
		char symbol = buffer[x];
		if (c == symbol){
			count_c ++;
		}
	}

	return count_c;
}



// Get the position of c when count of c reaches the required count
int inverse_occ1(ifstream& bin, int count, char c, int bwt_size){

	int count_c = 0;

	char buffer [bwt_size];

	// get to the beginning of the file
	bin.seekg(ios::beg);
	bin.read(buffer, bwt_size);

	for (int x = 0; x < bwt_size; x++) {
		char symbol = buffer[x];
		if (c == symbol){
			count_c ++;
		}

		if (count_c == count){
			return x;
			break;
		}
	}
}


void backward_search1(char* bwt_file, char* pattern, string other_patterns[2], int bwt_size) {

	// 1. generate occurrence table (only need size of 1 sub index - 128*4 bytes)
	int occ_idx[128] = {0};

	char symbol;

	ifstream in(bwt_file, ios::in);
	while(!in.eof()){
		symbol = in.get();
		occ_idx[symbol]++;
	}
	in.close();


	// 2. Using the occurrence index to build C_table
	int count = 0;

	// for ascii[9], it should be recorded with 0

	// for ascii[10], it should be recorded with counts for ascii[9]
	count+= occ_idx[9];
	C_list[10] = count;

	// for ascii[13], it should be recorded with counts for ascii[9&10]
	count+= occ_idx[10];
	C_list[13] = count;

	count+= occ_idx[13];
	// for other ascii[32-126], do similar things
	for (int x = 32; x < 127; x++){
		C_list[x] = count;
		count+= occ_idx[x];
	}


	// 3. Do backward search based on C_list
	string p = pattern;
	int p_length = p.size();

	// open bwt file:
	ifstream bin(bwt_file, ios::in);

	// Initialize i, start, and end:
	int i = p_length -1;
	char c = p[i];

	int start = C_list[c];
	int end = C_list[c+1] - 1;

	while(i >= 1 && start <= end){
		c = p[i-1];
		start = C_list[c] + occ1(bin, start-1, c);
		end = C_list[c] + occ1(bin, end, c) -1 ;
		i = i-1;
	}

	if (end < start){
		//cout << "No match!";
	}

	// If there are matches,
	// based on the matches do both backward decode and forward decode to get output:
	else{
		//cout << start << ":" << end;

		// for each position from start to end, do:
		for (int j = start; j < end+1; j++){
			// Use Result string to store the result
			string result = "";
			result += p.at(0);



			// 1. backward decode:

			// Use flag to determine is "]" is met during backward search
			bool backward_flag = false;

			char L_c = 0; // Initialize a char which will never be met
			int F_c_pos = j;

			while (L_c != '['){
				// 1.1 get the j-th position char in BWT(L) file:
				bin.seekg(F_c_pos, ios::beg);
				bin.read(&L_c, sizeof(char));

				//Add the found char before current result
				result = L_c + result;

				// Set flag to be true once ']' is met
				if (L_c == ']'){
					backward_flag = true;
				}

				// 1.2 use Occ to get the number of L_c before L_c in BWT file (position of L_c)
				int L_c_pos = occ1(bin, F_c_pos, L_c);

				// 1.3 Get the position of the previous L_c in C_list(F):
				F_c_pos = C_list[L_c] + L_c_pos -1;
			}


			if (backward_flag == false){
				continue;
			}


			// If everything's fine with backward decode,
			// 2. Do Forward Decode:

			char Fc = p.at(0);
			int Fc_pos = j;


			while (true){
				// 2.1 get the position of start-end in F list:
				int Lc_pos = Fc_pos - C_list[Fc] + 1;

				// 2.2 use Inverse_occ to know the position of F_c;
				Fc_pos = inverse_occ1(bin, Lc_pos, Fc, bwt_size);

				// 2.3 get char at Fc_position
				for (int y = 9; y < 128; y++){
					if (C_list[y] <= Fc_pos && C_list[y+1]> Fc_pos){
						Fc = y;
						break;
					}
				}

				if (Fc == '['){
					break;
				}
				else{
					result += Fc;
				}

			}


			// 3. check if the matched records contain 2 other patterns):

			string p2 = other_patterns[0];
			string p3 = other_patterns[1];

			string ext_result = result.substr(result.find("]") + 1);


			if ( ext_result.find(p2)== string::npos){
				continue;
			}
			else {
				if ( ext_result.find(p3)== string::npos){
					continue;
				}
			}


			// 4. Output
			cout << result << '\n';

		}

	}


}


int get_bwt_size(char *bwt){
	ifstream in(bwt, ios::in);
	in.seekg(0, ios::end);
    int bwt_size = in.tellg();
    in.close();
    return bwt_size;
}



int generate_idx(char* bwt_file, char* index_file) {

	unsigned int sub_idx[128] = {0};

	ifstream bwt_in(bwt_file, ios::in);
	bwt_in.seekg(ios::beg);
	ofstream idx_out(index_file, ios::binary);

	int Num_sub_idx = 0;

	char buffer [sub_idx_size];

	while(true){
		bwt_in.read(buffer, sub_idx_size);

		if (bwt_in.gcount() < sub_idx_size){
			break;
		}

		else if (bwt_in.gcount() == sub_idx_size){

			for (int x = 0; x < sub_idx_size; x++){
				char tmp_char = buffer[x];
				sub_idx[tmp_char]++;
			}

			// write current c_list to index file:
			for (int y = 0; y < 128; y++){
				idx_out.write(reinterpret_cast<char *> (&sub_idx[y]), sizeof(unsigned int));
			}
			Num_sub_idx += 1;


		}

	}

	bwt_in.close();
	idx_out.close();

	return Num_sub_idx;

}



// Use index and BWT to get C_list
void generate_C_list(char* bwt_file, char* index_file, int num_of_subs, int bwt_size){

	unsigned int occ_idx1[128] = {0};

	ifstream idx_in(index_file, ios::binary);

	if (num_of_subs){
		unsigned start_pos = (num_of_subs -1) * 512;

		// Read index and put it into occ_idx1:
		idx_in.seekg(start_pos, ios::beg);
		idx_in.read(reinterpret_cast<char *> (&occ_idx1), sizeof(unsigned int)*128);

	}
	idx_in.close();


	ifstream bwt_in(bwt_file, ios::in);

	// jump to the position in BWT where not covered in index:
	bwt_in.seekg(num_of_subs * sub_idx_size, ios::beg);

	char symbol;

	// Use buffer
	int buffer_size = bwt_size - num_of_subs * sub_idx_size;
	char buffer [buffer_size];

	bwt_in.read(buffer, buffer_size);

	for (int v = 0; v < buffer_size; v++) {
		symbol = buffer[v];
		occ_idx1[symbol]++;
	}
	bwt_in.close();


	// Using the occurrence index to build C_table
	int count = 0;

	count+= occ_idx1[9];
	C_list[10] = count;

	count+= occ_idx1[10];
	C_list[13] = count;

	count+= occ_idx1[13];

	for (int x = 32; x < 127; x++){
		C_list[x] = count;
		count+= occ_idx1[x];
	}


}


int occ2(ifstream& bin, ifstream& idx_in, int pos, char c){
	unsigned int count_c = 0;

	// Check which sub_index the position is in:
	int which_sub = pos/sub_idx_size;

	int z = 0;

	if (which_sub){

		z = which_sub * sub_idx_size;

		// Get the nearest occurrence of c
		unsigned start_pos = (which_sub -1) * 512;
		idx_in.seekg(start_pos + c * sizeof(unsigned int), ios::beg);
		idx_in.read(reinterpret_cast<char *> (&count_c), sizeof(unsigned int));

		bin.seekg(which_sub * sub_idx_size, ios::beg);

	}
	else {
		bin.seekg(0, ios::beg);
	}

	char buffer [pos-z+1];
	bin.read(buffer, pos-z+1);

	// Count the overhead of c in BWT and add to c_count:
	for (int i = 0; i < pos-z+1; i++) {
		char symbol = buffer[i];
		if (c == symbol){
			count_c ++;
		}
	}

	return count_c;
}



// Return the position of c when count of c reaches the required count
unsigned int inverse_occ2(ifstream& bin, ifstream& idx_in, unsigned int count, char c, int num_of_subs, int bwt_size){

	// 1. Get approximate position in Index file:
	unsigned int count_c = 0;

	int low = 0;
	int high = num_of_subs -1;
	int x;

	// Binary search
	while(low <= high){
		int mid = (low + high)/2;

		idx_in.seekg(mid*512 + c * sizeof(unsigned int), ios::beg);
		idx_in.read(reinterpret_cast<char *> (&count_c), sizeof(unsigned int));

		if (count_c >= count){
			high = mid;
		}
		else if (count_c < count){
			low = mid;
		}


		if (high - low ==1){

			unsigned int low_value = 0;
			idx_in.seekg(low*512 + c * sizeof(unsigned int), ios::beg);
			idx_in.read(reinterpret_cast<char *> (&low_value), sizeof(unsigned int));


			unsigned int high_value = 0;
			idx_in.seekg(high*512 + c * sizeof(unsigned int), ios::beg);
			idx_in.read(reinterpret_cast<char *> (&high_value), sizeof(unsigned int));


			// case1:
			if (low_value >= count){
				x = low;
				count_c = 0;
				break;
			}

			// case2:
			if (high_value < count){
				x = high + 1;
				count_c = high_value;
				break;
			}

			// case3:
			if (low_value < count && high_value >= count){
				x = high;
				count_c = low_value;
				break;
			}

		}

		// Deal with situation where only has 1 block
		else if (high == low){
			unsigned int value = 0;
			idx_in.seekg(low*512 + c * sizeof(unsigned int), ios::beg);
			idx_in.read(reinterpret_cast<char *> (&value), sizeof(unsigned int));

			if (value >= count){
				x = low;
				count_c = 0;
				break;
			}

			if (value < count){
				x = low+1;
				count_c = value;
				break;
			}

		}
		//


	}



	// 2. get to the position in BWT
	bin.seekg(x*sub_idx_size, ios::beg);


	// CASE 1:
	if (bwt_size >= (x+1)*sub_idx_size){

		char buffer [sub_idx_size];
		bin.read(buffer, sub_idx_size);


		for (int v = 0; v < sub_idx_size; v++) {

			char symbol = buffer[v];
			if (c == symbol){
				count_c ++;
			}

			if (count_c == count){
				return x*sub_idx_size + v;
				break;
			}
		}

	}

	// CASE2:
	else {
		int buffer_size = bwt_size - x*sub_idx_size;
		char buffer [buffer_size];
		bin.read(buffer, buffer_size);

		for (int v = 0; v < buffer_size; v++) {

			char symbol = buffer[v];
			if (c == symbol){
				count_c ++;
			}

			if (count_c == count){
				return x*sub_idx_size + v;
				break;
			}
		}

	}



}



void backward_search2(char* bwt_file, char* index_file, char* pattern, string other_patterns[2], int num_of_subs, int bwt_size) {

	// 1. Do backward search based on C_list
	string p = pattern;
	int p_length = p.size();

	// open bwt file & index file:
	ifstream bin(bwt_file, ios::in);
	ifstream idx_in(index_file, ios::binary);

	// Initialize i, start, and end:
	int i = p_length -1;
	char c = p[i];

	int start = C_list[c];
	int end = C_list[c+1] - 1;

	while(i >= 1 && start <= end){
		c = p[i-1];
		start = C_list[c] + occ2(bin, idx_in, start-1, c);
		end = C_list[c] + occ2(bin, idx_in, end, c) -1 ;
		i = i-1;
	}

	if (end < start){
		//cout << "No match!";
	}
	else {
		//cout << start << ':' << end;

		// From start to end, do:
		for (int s = start; s < end+1; s++){
			// Use Result string to store the result
			string result = "";
			result += p.at(0);

			// 1. backward decode:

			// Use flag to determine is "]" is met during backward search
			bool backward_flag = false;


			char L_c = 0; // Initialize a char which will never be met
			int F_c_pos = s;

			while (L_c != '['){
				// 1.1 get the s-th position char in BWT(L) file:
				bin.seekg(F_c_pos, ios::beg);
				bin.read(&L_c, sizeof(char));

				//Add the found char before current result
				result = L_c + result;

				// Set flag to be true once ']' is met
				if (L_c == ']'){
					backward_flag = true;
				}

				// 1.2 use Occ to get the number of L_c before L_c in BWT file (position of L_c)
				int L_c_pos = occ2(bin, idx_in, F_c_pos, L_c);

				// 1.3 Get the position of the previous L_c in C_list(F):
				F_c_pos = C_list[L_c] + L_c_pos -1;
			}


			if (backward_flag == false){
				continue;
			}

			// If nothing going wrong,

			// 2. Do Forward Decode:
			char Fc = p.at(0);
			unsigned int Fc_pos = s;

			while (true){
				// 2.1 get the position of start-end in F list:
				unsigned int Lc_pos = Fc_pos - C_list[Fc] + 1;

				// 2.2 use Inverse_occ to know the position of F_c;
				Fc_pos = inverse_occ2(bin, idx_in, Lc_pos, Fc, num_of_subs, bwt_size);

				//cout << Fc_pos <<endl;


				// 2.3 get char at Fc_position
				for (int y = 9; y < 128; y++){
					if (C_list[y] <= Fc_pos && C_list[y+1]> Fc_pos){
						Fc = y;
						//cout << Fc;
						break;
					}
				}

				if (Fc == '['){
					break;
				}
				else{
					result += Fc;
				}

			}



			// 3. check if the matched records contain 2 other patterns):

			string p2 = other_patterns[0];
			string p3 = other_patterns[1];

			string ext_result = result.substr(result.find("]") + 1);

			if ( ext_result.find(p2)== string::npos){
				continue;
			}
			else {
				if ( ext_result.find(p3)== string::npos){
					continue;
				}
			}


			// 4. Output
			cout << result << '\n';


		}

	}


}



int main(int argc, char* argv[]) {

	// ------------------------1. Dealing with arguments-----------------------------//
	// 1.1 InputFile //
	char* bwt_file = argv[1];

	// 1.2 Index file to be built //
	char* index_file = argv[2];

	// 1.3 Accept 1 to 3 searching terms: backward search based on longest search term //
	// put longest term in char* search_term, others into list of other_terms //
	char* search_term;
	string other_patterns[2];

	// only one search term
	search_term = argv[3];

	// two search terms
	if(argc > 4){
		string st = search_term;
		string s2 = argv[4];

		if (s2.length()>st.length()){
			other_patterns[0] = search_term;
			search_term = argv[4];
		}
		else{
			other_patterns[0] = argv[4];
		}
	}

	// three search terms
	if(argc > 5){
		string st = search_term;
		string s3 = argv[5];

		if (s3.length()>st.length()){
			other_patterns[1] = search_term;
			search_term = argv[5];
		}
		else{
			other_patterns[1] = argv[5];
		}
	}
	// -----------------------(End 1) Dealing with arguments ---------------------------//



	// ------------------------------2. Backward Search  --------------------------------//
	// Get the size of given BWT file:
	int bwt_size = get_bwt_size(bwt_file);

	//cout << "BWT size: " << bwt_size <<endl;

	// determine how many sub indexes are needed
	int num_of_subs;


	// 2.1 If BWT size <= pre-defined bytes boundary, directly giving output
	if (bwt_size <= sub_idx_size){
		backward_search1(bwt_file, search_term, other_patterns, bwt_size);
	}

	// 2.2 else, build external index file and use index file info to make output
	else {

		ifstream in_idx(index_file);

		// Check if index file already exits:
		if (in_idx){
			in_idx.seekg(0, ios::end);
			int size = in_idx.tellg();
			num_of_subs = size / 512;

			in_idx.close();
		}

		// if index does not exist, create a new one
		else {
			in_idx.close();
			num_of_subs = generate_idx(bwt_file, index_file);

		}

		//cout << num_of_subs;

		// Use external index file and original BWT to build C_list:
		generate_C_list(bwt_file, index_file, num_of_subs, bwt_size);

		// backward_search2:
		backward_search2(bwt_file, index_file, search_term, other_patterns, num_of_subs, bwt_size);

	}



	//backward_search(bwt_file, index_file, search_term1);


	// ----------------------------(End 2) Backward Search  ------------------------------//

	return 0;
}




