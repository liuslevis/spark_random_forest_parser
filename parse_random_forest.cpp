#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <numeric>
using namespace std;

typedef struct node {
    string cmp = string("");
    double weight = 0.0f; // for root
    long tree_i = 0; // for root
    long predict = -1; // for leaf
    long feature = -1; // for node
    double cmpVar = -1.0f;
    struct node *if_node = NULL;
    struct node *else_node = NULL;
} Node;

typedef vector<Node> Forest;

bool has_words(string const& str, string const& words) {
    if (str.find(words.c_str()) != string::npos) {
        return true;
    }
    return false;
}

bool is_prefix( string const& lhs, string const& rhs ) {
    return equal(
        lhs.begin(),
        lhs.begin() + min( lhs.size(), rhs.size() ),
        rhs.begin() );
}

// "this is demo 3 and somehting 4 you", "something" -> 4
int get_int_from_string_after_word(string s, string after_word) {
    size_t pos = s.find(after_word.c_str());
    if (pos == string::npos) {
        cout << "get_int_from_string_after_word out of range:" << s << " " << after_word << endl;
        return -1;
    }
    const char *str = s.substr(pos).c_str();
    while (!(*str >= '0' && *str <= '9') && (*str != '-') && (*str != '+')) str++;
    int number;
    if (sscanf(str, "%d", &number) == 1) {
        return number;
    }
    return -1; 
}
// "this is demo 3 and somehting 4 you", "demo" -> 3.0
double get_double_from_string_after_word(string s, string after_word) {
    size_t pos = s.find(after_word.c_str());
    if (pos == string::npos) {
        cout << "get_double_from_string_after_word out of range:" << s << " " << after_word << endl;
        return -1;
    }
    const char *str = s.substr(pos).c_str();
    while (!(*str >= '0' && *str <= '9') && (*str != '-') && (*str != '+')) str++;
    double num;
    if (sscanf(str, "%lf", &num) == 1) {
        return num;
    }
    return -1; 
}

int total_tree_for_line(string s) {
    return get_int_from_string_after_word(s, string("with "));
}

int cur_tree_for_line(string s) {
    return get_int_from_string_after_word(s, string("Tree "));
}

double cur_tree_weight_for_line(string s) {
    return get_double_from_string_after_word(s, "weight ");
}

string get_cmp_from_line(string line) {
    vector<string> cmps = {string(">="), string("<="), string(">"), string("<"), string("="), string("==")};
    for (auto cmp : cmps) {
        if (has_words(line, cmp)) return cmp;    
    }
    return string("");
}

vector<string>::iterator iterator_last_line(vector<string> lines) {
    vector<string>::iterator i = lines.begin();
    while(i != lines.end()) {
         ++i;
    }
    return i;
}

// return index of first "If"/"Else"/word line
int index_first_line_begin_with_word(vector<string> lines, string word) {
    int not_found = 99999;
    int min_pos = not_found;
    int min_index = not_found;
    for (int i = 0; i < lines.size(); i++) {
        string line = lines[i];
        size_t pos = line.find(word.c_str());
        if (pos != string::npos) {
            if (min_pos >= pos) {
                min_pos = pos;
                min_index = i;
            }
        }
    }
    return min_index == not_found ? -1 : min_index;
}

void gen_node_from_lines(Node *node, vector<string> lines) {
    // cout<<"gen_node_from_lines:"<<lines.size() << *(lines.begin()) <<endl;
    if (lines.size() < 1) return;
    string predict_string("Predict: ");
    string feature_string("feature ");

    if (has_words(lines[0], predict_string)) {
        node->predict = get_int_from_string_after_word(lines[0], predict_string);
        return;
    } 

    node->cmp = get_cmp_from_line(lines[0]);

    node->feature = get_int_from_string_after_word(lines[0], feature_string);

    node->cmpVar = get_double_from_string_after_word( lines[0], node->cmp);
    int if_begin = 1 + index_first_line_begin_with_word(lines, string("If"));
    int if_end = index_first_line_begin_with_word(lines, string("Else"));
    int else_begin = 1 + index_first_line_begin_with_word(lines, string("Else"));
    int else_end = lines.size();

    if (if_begin > if_end && else_begin > else_end) {
        cout<<"index calc error"<<endl;
    }
    
    vector<string>   if_lines(lines.begin() + if_begin,   lines.begin() + if_end);    
    vector<string> else_lines(lines.begin() + else_begin, lines.begin() + else_end);
    Node *if_node   = new Node();
    Node *else_node = new Node();
    node->if_node = if_node;
    node->else_node = else_node;

    gen_node_from_lines(if_node, if_lines);
    gen_node_from_lines(else_node, else_lines);

}

// return -1 if failed to gen else return 0 succ
int gen_tree_from_lines(Node *root, vector<string> lines) {
    root->weight = cur_tree_weight_for_line(lines[0]);
    root->tree_i = cur_tree_for_line(lines[0]);
    // if ((lines.size() - 1) % 2 != 0) {
        // cout<< "parse error: num of lines should be even! tree:" << *lines.begin() << endl;
        // return -1;
    // }
    vector<string> tree_lines(lines.begin() + 1, lines.end());
    gen_node_from_lines(root, tree_lines);
    return 0;
}

void print_node(Node *node, long depth) {
    string indent = string(depth, ' ');
    Node *if_node = node->if_node;
    Node *else_node = node->else_node;

    if (node->predict > -1) {
        cout << indent << "Predict:" << node->predict << endl;
    } else if (if_node != NULL && else_node != NULL) {
        cout<< indent << "If feature:" << node->feature << node->cmp << node->cmpVar << endl;
        print_node(if_node,   depth + 1);
        cout<< indent << "Else:" << endl;
        print_node(else_node, depth + 1);    
    }
}

void print_tree(Node *root) {
    cout << "Parsed Tree:" << endl;
    cout << "Tree: " << root->tree_i << " weight:" << root->weight << endl;
    print_node(root, 0);    
}

void print_tree_lines(vector<vector<string> > trees_lines) {
    for (auto tree_lines : trees_lines) {
        cout << endl;
        int cnt = 0;
        for (auto line : tree_lines) {
            cout<< cnt++ << ":" << line << endl;    
        }
    }
}

Forest build_forest(vector<vector<string> > trees_lines) {
    Forest tree_roots;
    int cnt = 0;
    for (auto i = trees_lines.begin() + 1; i != trees_lines.end(); ++i) {
        vector<string> tree_lines = *i;
        cnt++;
        Node root;
        int ret = gen_tree_from_lines(&root, tree_lines);
        if (ret == 0) {
            tree_roots.push_back(root);
        }
    }
    return tree_roots;
}

void print_forest(Forest forest) {
    for (auto root : forest) {
        print_tree(&root);
    }
}

vector<vector<string> > parse_trees_lines_from_forest_lines(vector<string> forest_lines) {
    vector<vector<string> > trees_lines;
    int total_tree = -1;
    string line;
    int i = 0;
    for (i = 0; i < forest_lines.size(); ++i) {
        string line = forest_lines[i];
        if (total_tree == -1) {
            total_tree = total_tree_for_line(line);
            cout<< "will parse num of tree:" << total_tree << endl;
            break;
        }
    }
    if (total_tree < 0) {
        cout << "can't find trees" << endl;
        return trees_lines;
    }

    vector<string> tree_lines;
    for (; i < forest_lines.size(); ++i) {
        string line = forest_lines[i];
        if (line.find(string("Tree ")) != string::npos) {
            if (tree_lines.size() > 0) {
                trees_lines.push_back(tree_lines);
                tree_lines.clear();
            }
        }
        tree_lines.push_back(line);
    }
    trees_lines.push_back(tree_lines); // last tree
    cout<< "did parse num of tree:" << trees_lines.size() - 1 << endl;

    return trees_lines;
}

vector<string> lines_from_file(string path) {
    vector<string> lines;
    ifstream infile;
    string line;
    infile.open(path.c_str());
    int cnt = 0;
    while (!infile.eof()) {
        getline(infile, line);
        lines.push_back(line);
    }
    infile.close();
    return lines;
}

vector<string> split_string(string s) {
    vector<string> tokens;
    istringstream iss(s);
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(tokens));
    return tokens;
}

void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

double predict_value(Node *root, unordered_map<long, double> feature_var) {
    Node *node = root;
    while (node->predict == -1) {
        long feature = node->feature;
        double featureVar = 0;
        if (feature_var.find(feature) != feature_var.end()) {
            featureVar = feature_var[feature];
        }
        double cmpVar = node->cmpVar;
        string cmp = node->cmp;
        bool go_if = false;
        if (string("==").compare(cmp) == 0 || string("=").compare(cmp) == 0) {
            if (featureVar == cmpVar) go_if = true;
        } else if (string(">").compare(cmp) == 0) {
            if (featureVar > cmpVar) go_if = true;
        } else if (string(">=").compare(cmp) == 0) {
            if (featureVar >= cmpVar) go_if = true;
        } else if (string("<").compare(cmp) == 0) {
            if (featureVar < cmpVar) go_if = true;
        } else if (string("<=").compare(cmp) == 0) {
            if (featureVar <= cmpVar) go_if = true;
        } else {
            cout<< "predict_value: unknown cmp:" << cmp <<endl;
            break;
        }
        node = go_if ? node->if_node : node->else_node;
    }
    return node->predict;
}

// [ (pred,weight), ...]
vector<tuple<long, double> > forest_pred_weights_for_line(string line, Forest forest) {
    vector<tuple<long, double> > pred_weights;
    vector<string> tokens = split_string(line);
    unordered_map<long, double> feature_var;

    long feature = -1;
    double value = 0.0;
    for (int i = 1; i < tokens.size(); ++i) {
        if (i % 2 == 1) {
            // feature e.g. "2:""
            feature = atoi(split(tokens[i], ':')[0].c_str());
        } else {
            // value e.g. "0.25:"
            value = atof(tokens[i].c_str());
            feature_var[feature] = value;
        }
    }
    vector<double> tree_predict;
    for (Node root : forest) {
        // print_tree(&root);
        double predict = predict_value(&root, feature_var);
        pred_weights.push_back(make_tuple(predict, root.weight));
    }
    for (auto predict : tree_predict) {
        cout<<"predict:"<<predict<<endl;
    }
    return pred_weights;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        cout << "usage: ./parse_random_forest forest_description.txt forest_feed.libsvm" << endl;
        exit(-1);
    }
    vector<string> feature_lines = lines_from_file(string(argv[2]));
    vector<string> forest_lines = lines_from_file(string(argv[1]));
    vector<vector<string> > trees_lines = parse_trees_lines_from_forest_lines(forest_lines);
    // print_tree_lines(trees_lines);
    Forest forest = build_forest(trees_lines);
    // print_forest(forest);
    for (string line : feature_lines) {
        if (line.length() < 1) continue;
        vector<tuple<long, double> > pred_weights = forest_pred_weights_for_line(line, forest);
        cout << "input libsvm:" << line << endl;
        for (int i = 0; i < pred_weights.size(); ++i) {
            auto pred_weight = pred_weights[i];
            cout << "\t" << "Tree:" << i << " prediction:" << get<0>(pred_weight) << " weight:" << get<1>(pred_weight) << endl;
        }
    }
    return 0;
}
