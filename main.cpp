#include <bits/stdc++.h>
using namespace std;

static const char* DB_FILE = "kvstore.bin";

struct FastIO {
    static void setup() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
    }
};

// Persisted format:
// uint32_t key_count
// repeat key_count times:
//   uint16_t klen
//   char key[klen]
//   uint32_t vcount
//   int32_t values[vcount] (sorted ascending)

static bool load_store(unordered_map<string, vector<int>>& mp) {
    mp.clear();
    ifstream fin(DB_FILE, ios::binary);
    if (!fin.good()) return false;
    uint32_t key_cnt = 0;
    fin.read(reinterpret_cast<char*>(&key_cnt), sizeof(key_cnt));
    if (!fin) return false;
    mp.reserve(key_cnt * 2 + 1);
    for (uint32_t i = 0; i < key_cnt; ++i) {
        uint16_t klen = 0;
        fin.read(reinterpret_cast<char*>(&klen), sizeof(klen));
        if (!fin) return false;
        string key;
        key.resize(klen);
        if (klen) fin.read(&key[0], klen);
        uint32_t vcnt = 0;
        fin.read(reinterpret_cast<char*>(&vcnt), sizeof(vcnt));
        if (!fin) return false;
        vector<int> vals;
        vals.resize(vcnt);
        if (vcnt) fin.read(reinterpret_cast<char*>(vals.data()), vcnt * sizeof(int32_t));
        if (!fin) return false;
        mp.emplace(std::move(key), std::move(vals));
    }
    return true;
}

static bool save_store(const unordered_map<string, vector<int>>& mp) {
    ofstream fout(DB_FILE, ios::binary | ios::trunc);
    if (!fout.good()) return false;
    uint32_t key_cnt = static_cast<uint32_t>(mp.size());
    fout.write(reinterpret_cast<const char*>(&key_cnt), sizeof(key_cnt));
    for (const auto& kv : mp) {
        const string& key = kv.first;
        const vector<int>& vals = kv.second;
        uint16_t klen = static_cast<uint16_t>(key.size());
        fout.write(reinterpret_cast<const char*>(&klen), sizeof(klen));
        if (klen) fout.write(key.data(), klen);
        uint32_t vcnt = static_cast<uint32_t>(vals.size());
        fout.write(reinterpret_cast<const char*>(&vcnt), sizeof(vcnt));
        if (vcnt) fout.write(reinterpret_cast<const char*>(vals.data()), vcnt * sizeof(int32_t));
    }
    return true;
}

int main() {
    FastIO::setup();

    unordered_map<string, vector<int>> store;
    load_store(store); // if fail, start empty

    int n;
    if (!(cin >> n)) return 0;
    string cmd;
    string key;
    int val;

    store.reserve((size_t)min(300000, n) * 2 + 1);

    for (int i = 0; i < n; ++i) {
        cin >> cmd;
        if (cmd == "insert") {
            cin >> key >> val;
            auto& vec = store[key];
            auto it = lower_bound(vec.begin(), vec.end(), val);
            if (it == vec.end() || *it != val) vec.insert(it, val);
        } else if (cmd == "delete") {
            cin >> key >> val;
            auto it = store.find(key);
            if (it != store.end()) {
                auto& vec = it->second;
                auto itv = lower_bound(vec.begin(), vec.end(), val);
                if (itv != vec.end() && *itv == val) {
                    vec.erase(itv);
                    if (vec.empty()) store.erase(it);
                }
            }
        } else if (cmd == "find") {
            cin >> key;
            auto it = store.find(key);
            if (it == store.end() || it->second.empty()) {
                cout << "null\n";
            } else {
                const auto& vec = it->second;
                for (size_t j = 0; j < vec.size(); ++j) {
                    if (j) cout << ' ';
                    cout << vec[j];
                }
                cout << '\n';
            }
        } else {
            string line; getline(cin, line);
        }
    }

    save_store(store);
    return 0;
}
