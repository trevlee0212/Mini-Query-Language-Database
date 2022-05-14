//  Created by Trevor Lee on 10/28/21.
//  Copyright © 2021 Trevor Lee. All rights reserved.
//

#include "log_entry.h"
#include <string.h>
#include "xcode_redirect.hpp"
#include <getopt.h>
#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include<algorithm>
#include <unordered_set>
#include <string.h>

using namespace std;

 bool LogComparator ( const log_entry& l1, const log_entry& l2) {
        if (l1.timestamp != l2.timestamp) {
            return (l1.timestamp < l2.timestamp);
        }
        
        auto res = strcasecmp(l1.category.c_str(), l2.category.c_str());
        if (res!=0){
            if (res < 0){
                return true;
            }
            return false;
        }
            
        else {
            return (l1.entryID < l2.entryID);
        }
    }


 bool findTime (const log_entry& l1,const log_entry& l2) {
            return (l1.timestamp < l2.timestamp);
    }


class log_management{
private:
    vector<log_entry> master_log;

    vector<size_t> key_list;
    deque<size_t> excerpt_list;
    unordered_map<string, vector<size_t> > index_category;
    unordered_map<string, vector<size_t> > index_word;
    vector<int> id_list;
    bool prev_search = false;
    char recent_cmd = '\0';
    pair<int, int> time_pair;
    string c_cmd_string;
    
public:
    
    
    //INPUT
    // read the input to put into master_log
    void create_master_log(string& input){
        string time;
        string category;
        string msg;
        int32_t i = 0;
        ifstream myfile;
        myfile.open(input);
        //myfile.open("sample-log.txt");
        //myfile.open("spec-test-log.txt");
        while (getline(myfile, time, '|')){
            getline(myfile, category, '|');
            getline(myfile, msg);
            master_log.emplace_back(i, time, category, msg);
            ++i;
        }
        cout << i << " entries read\n";
        sort(master_log.begin(), master_log.end(), LogComparator);
    }
    
    // sort master_log by 1. time 2. category 3. ID
    void make_index_category_word(){
        id_list.resize(master_log.size(), -1);
        for (size_t i =0; i<master_log.size();++i){
            id_list[(unsigned long)master_log[i].entryID] = (int32_t)i;
            string tmp = master_log[i].category;
            transform(master_log[i].category.begin(), master_log[i].category.end(), tmp.begin(), ::tolower);
            index_category[" "+tmp].push_back(i);
            string s = tmp + " " + master_log[i].message;
            // change non alnum to a space + change upper case to lower case
            for (size_t i=0;i < s.length(); ++i){
                if (!isalnum(s[i])){
                    s[i] = ' ';
                }
                else if (isupper(s[i])){
                    s[i] = (char)tolower(s[i]);
                }
            }
             // substr
            istringstream ss(s);
            string word;
            while (ss >> word){
                string rndm = word;
                if (index_word.find(rndm)==index_word.end()){
                    index_word[rndm].push_back(i);
                }
                else if (index_word[rndm].back()!=i){
                    index_word[rndm].push_back(i);
                }
            }
        }
    }

    //ALGO
    
    //MISCELLANEOUS
    
    void num_sign_cmd(){
        string trash;
        getline(cin, trash);
    }
    
    //Excerpt List Editing Commands
    
    //Append log entry
    //Append the log entry from position <integer> in the master log file onto the end of the excerpt list.
    //Print statement
    void a_cmd(){
        
        int index_input;
        cin >> index_input;
        
         // invalid input
        if (index_input >= (int)master_log.size() || index_input < 0){
            cerr << "invalid number input A \n";
            return;}
        excerpt_list.push_back((size_t)id_list[(unsigned long)index_input]);
        cout << "log entry " << index_input << " appended\n";
    }
    
    // Add all log entries returned by the most recent previous search (commands t, m, c, or k) to the end of the excerpt list.
    void r_cmd(){
        if (recent_cmd == 'm' || recent_cmd == 't'){
            for (size_t i = (size_t)time_pair.first; i< (size_t)time_pair.second;++i){
                excerpt_list.push_back(i);
            }
            cout << time_pair.second-time_pair.first << " log entries appended\n";
            return;
        }
        else if (recent_cmd == 'c'){
            for (size_t i = 0; i< index_category[c_cmd_string].size();++i){
                excerpt_list.push_back(index_category[c_cmd_string][i]);
            }
            cout << index_category[c_cmd_string].size() << " log entries appended\n";
            return;
            
        }
        if (recent_cmd == 'k'){
            for (size_t i=0; i< key_list.size();++i){
                excerpt_list.push_back(key_list[i]);
            }
            cout << key_list.size() << " log entries appended\n";
            return;
        }
        //else if (prev_search == true) {cout << "0 log entries appended\n";}
    }
    
    //Remove the excerpt list entry at position <integer>.
    void d_cmd(){
        int input;
        cin >> input;
        
        // invalid input
        if (input >= (int)excerpt_list.size() || input < 0){
            cerr << "invalid number input D\n";
            return;}
        
        excerpt_list.erase(excerpt_list.begin()+(input));
        cout << "Deleted excerpt list entry " << input << "\n";
    }
    
    //Move the excerpt list entry at position <integer> to the beginning of the excerpt list.
    void b_cmd(){
        int input;
        cin >> input;
        
        // invalid input
        if (input >= (int)excerpt_list.size() || input < 0){
            cerr << "invalid number input B \n";
            return;}
        
        excerpt_list.push_front(excerpt_list[(unsigned long)input]);
        excerpt_list.erase(excerpt_list.begin()+input+1);
        
        cout << "Moved excerpt list entry " << input << "\n";
    }
    
    //Move the excerpt list entry at position <integer> to the end of the excerpt list
    void e_cmd(){
        int input;
        cin >> input;
        
        // invalid input
        if (input >= (int)excerpt_list.size() || input < 0){
            cerr << "invalid number input E \n";
            return;}
        
        excerpt_list.push_back(excerpt_list[(unsigned long)input]);
        excerpt_list.erase(excerpt_list.begin()+(input));
        cout << "Moved excerpt list entry " << input << "\n";
    }
    
    // Remove all entries from the excerpt list.
    void l_cmd(){
        if (excerpt_list.empty()){
            cout << "excerpt list cleared\n";
            cout << "(previously empty)\n";
            return;
        }
        
        cout << "excerpt list cleared\nprevious contents:\n";
        cout << "0|";
        print_log(master_log[excerpt_list[0]]);
        cout << "...\n";
        cout << excerpt_list.size()-1 << "|";
        print_log(master_log[excerpt_list[excerpt_list.size()-1]]);
        excerpt_list.clear();
        
    }
    
    //Sort each entry in the excerpt list
    void s_cmd(){
        if (excerpt_list.empty()){
            cout << "excerpt list sorted\n";
            cout << "(previously empty)\n";
            return;
        }
        
        cout << "excerpt list sorted\nprevious ordering:\n";
        cout << "0|";
        print_log(master_log[excerpt_list[0]]);
        cout << "...\n";
        cout << excerpt_list.size()-1 << "|";
        print_log(master_log[excerpt_list[excerpt_list.size()-1]]);
        
        cout << "new ordering:\n";
        sort(excerpt_list.begin(), excerpt_list.end());
        cout << "0|";
        print_log(master_log[excerpt_list[0]]);
        cout << "...\n";
        cout << excerpt_list.size()-1 << "|";
        print_log(master_log[excerpt_list[excerpt_list.size()-1]]);
    }
    
    
    //Searching Commands
    void k_cmd(){
        string input = "";
        getline(cin, input);
        
        //split the keyword
        vector<string> ctnr;
        string keyword = "";
        for (size_t i=0; i< input.length(); ++i){
            if (isalnum(input[i])==false){
                if (keyword != ""){
                    ctnr.push_back(keyword);
                    keyword = "";}
            }
            else{
                keyword += (char)tolower(input[i]);
            }
        }
        
        if (keyword != ""){
            ctnr.push_back(keyword);
        }
        
        // start finding keyword appearance
        // key_list = vector I am storing the intersection in
        // index_word = unordered map for keyword search
        // ctnr = keyword from the input
        
        auto zero = index_word.find(ctnr[0]);
        if (zero == index_word.end()){
            cout << "Keyword search: " << 0 << " entries found\n";
            prev_search = true;
            recent_cmd = 'k';
            vector<size_t> empty_l;
            empty_l.swap(key_list);
            return;
        }
        
        key_list = index_word[ctnr[0]];
        
        vector<size_t> tmp;
        for (size_t i=1; i< ctnr.size();++i){
            auto it = index_word.find(ctnr[i]);
            if (it!=index_word.end()){
            set_intersection (it->second.begin(), it->second.end(), key_list.begin(), key_list.end(), back_inserter(tmp));
            key_list.swap(tmp);
            tmp.clear();
            }
            else {
                cout << "Keyword search: " << 0 << " entries found\n";
                prev_search = true;
                recent_cmd = 'k';
                key_list.clear();
                return;
            }
        }
        
        cout << "Keyword search: " << key_list.size() << " entries found\n";
        prev_search = true;
        recent_cmd = 'k';
    }
    
    
    // timestamps search
    // TODO: change to a pair of int
    void t_cmd(){
        string time_input = "";
        getline(cin, time_input);

        //check valid input
        if (time_input[0] == ' ' && time_input.length()==30 && time_input[15] == '|'){
            string low_str = time_input.substr(1,14);
            string high_str = time_input.substr(16,14);
            int64_t lower =  time_to_int(low_str);
            int64_t higher = time_to_int(high_str);

            log_entry lower_key;
            log_entry higher_key;
            lower_key.timestamp = lower;
            higher_key.timestamp = higher;
            auto low_it = lower_bound(master_log.begin(), master_log.end(), lower_key, LogComparator);
            auto high_it = upper_bound(master_log.begin(), master_log.end(), higher_key, findTime);
            
            
            time_pair = make_pair((int)(low_it - master_log.begin()), (int)(high_it - master_log.begin()));
            cout << "Timestamps search: " << time_pair.second -time_pair.first << " entries found\n";
            prev_search = true;
            recent_cmd = 't';
        }
        
        else {
            cerr << "invalid time input";
        }
    }
    
    // TODO: change to a pair of int
    // matching search
    void m_cmd(){
        string time_input;
        cin >> time_input;
        
        //check valid input
        if (time_input.length()==14){
            int64_t output = 0;
            output = time_to_int(time_input);
            log_entry key;
            key.timestamp = output;
            auto high_it = upper_bound(master_log.begin(), master_log.end(), key, findTime);
            auto low_it = lower_bound(master_log.begin(), master_log.end(), key, LogComparator);
            
            int low = (int)(low_it - master_log.begin());
            int high = (int)(high_it - master_log.begin());
            time_pair = make_pair(low, high);
            cout << "Timestamp search: " << high-low << " entries found\n";
            prev_search = true;
            recent_cmd = 'm';
            return;
        }
        else {
            cerr << "invalid time input";
        }
    }
    
    
    
    void c_cmd(){
        string string_input;
        getline(cin, string_input);
        transform(string_input.begin(), string_input.end(), string_input.begin(), ::tolower);
        auto it = index_category.find(string_input);
        if (it!=index_category.end()){
            cout << "Category search: " << (it->second).size() << " entries found\n";}
        else {
            cout << "Category search: " << 0 << " entries found\n";
        }
        c_cmd_string = string_input;
        prev_search = true;
        recent_cmd = 'c';
    }
    
    //OUTPUT COMMAND
    
    // Print the entries returned by the previous search, or nothing if no previous search has occurred.
    void g_cmd(){
        if (recent_cmd == 'm' || recent_cmd == 't'){
            for (size_t i = (size_t)time_pair.first; i< (size_t)time_pair.second;++i){
                print_log(master_log[i]);
            }
        }
        else if (recent_cmd == 'c'){
            for (size_t i = 0; i< index_category[c_cmd_string].size();++i){
                print_log(master_log[index_category[c_cmd_string][i]]);
            }
        }
        else if (recent_cmd == 'k'){
            for (size_t i =0; i<key_list.size();++i){
                print_log(master_log[key_list[i]]);
            }
        }
    }
    
    
    // Print the list of excerpt list entries in the excerpt list.
    void p_cmd(){
        for (size_t i =0; i<excerpt_list.size();++i){
            cout << i << "|";
            print_log(master_log[excerpt_list[i]]);
        }
        
    }
    
    
    // helper function: g_cmd & p_cmd
    void print_log(log_entry& input){
        cout << input.entryID << "|" << input.timestamp_output << "|" << input.category << "|" << input.message << "\n";
    }
    
    void execution(){
        char x = '\n';
        do {
            cout << "% ";
            cin >> x;
            menu(x);
        }while (x!= 'q');
    }
    
    void menu(char& input){
        switch (input){
            case 'k':
                k_cmd();
                break;
            case '#':
                num_sign_cmd();
                break;
            case 'a':
                a_cmd();
                break;
            case 's':
                s_cmd();
                break;
            case 'c':
                c_cmd();
                break;
            case 'p':
                p_cmd();
                break;
            case 'e':
                e_cmd();
                break;
            case 'r':
                r_cmd();
                break;
            case 'b':
                b_cmd();
                break;
            case 'g':
                g_cmd();
                break;
            case 't':
                t_cmd();
                break;
            case 'd':
                d_cmd();
                break;
            case 'm':
                m_cmd();
                break;
            case 'l':
                l_cmd();
                break;
            case 'q':
                break;
            default:
                cerr << "invalid command\n";
                num_sign_cmd();
                break;
        }
    }

};

int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc,argv);
    
    string tmp = argv[1];
    if (tmp == "-h" || tmp == "--help"){cout << "Error: invalid option" << "\n"; exit(0);}
    log_management g1;
    g1.create_master_log(tmp);
    g1.make_index_category_word();
    g1.execution();
    return 0;
}

