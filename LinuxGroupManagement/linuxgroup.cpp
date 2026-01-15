// Copyright 2025 Nick Pieroni All rights reserved.

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

// ------- Helper methods -------

// gets multiple inputs delimited by whitespace and returns the vector
std::vector<std::string> getInput(const std::string &prompt) {
    std::cout << prompt << "\n";

    // get the user input
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::vector<std::string> chars;
    std::string t;

    // add groups entered to vector
    while (iss >> t) chars.push_back(t);
    return chars;
}

// helper method to check if a string is a digit by checking each character
bool isNumber(const std::string &s) {
    for (char c : s) if (!isdigit(c)) return false;
    return !s.empty();
}

// loads lines from the specified file into a vector
std::vector<std::string> loadFile(const std::string &filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;

    // iterate through the specified file
    while (std::getline(file, line)) lines.push_back(line);
    return lines;
}

// ------- View functions -------

// iterates through the lines vector, and extracts login, pass, and uid
// the exctracted info gets mapped to loginToUid and uidToLogin
void parsePasswd(
    const std::vector<std::string> &lines,
    std::unordered_map<std::string, int> &loginToUid,
    std::unordered_map<int, std::string> &uidToLogin) {

    for (const auto &l : lines) {
        std::istringstream iss(l);
        std::string login, pass, uidStr;

        // get username, pass and uid from line
        if (!std::getline(iss, login, ':')) continue;
        if (!std::getline(iss, pass, ':')) continue;
        if (!std::getline(iss, uidStr, ':')) continue;
        int uid = std::stoi(uidStr);

        // map uid and login
        loginToUid[login] = uid;
        uidToLogin[uid] = login;
    }
}

// iterates through the lines vector, and extracts name, pass, and gid
// the exctracted info gets mapped to gidMap and nameMap
void parseGroup(
    const std::vector<std::string> &lines,
    std::unordered_map<int, std::pair<std::string,
        std::vector<std::string>>> &gidMap,
    std::unordered_map<std::string, int> &nameMap) {

    // iterate through lines
    for (const auto &l : lines) {
        std::istringstream iss(l);
        std::string name, pass, gidStr, members;

        // get name, pass and gid from line
        if (!std::getline(iss, name, ':')) continue;
        if (!std::getline(iss, pass, ':')) continue;
        if (!std::getline(iss, gidStr, ':')) continue;
        std::getline(iss, members, ':');

        // store as integer for the nameMap
        int gid = std::stoi(gidStr);

        std::vector<std::string> mems;
        std::istringstream ms(members);
        std::string m;

        // split members by comma
        while (std::getline(ms, m, ',')) if (!m.empty()) mems.push_back(m);

        // map gid and name
        gidMap[gid] = {name, mems};
        nameMap[name] = gid;
    }
}

// iterates through and prints a single groups info
void printGroup(
    int gid,
    const std::pair<std::string, std::vector<std::string>> &group,
    const std::unordered_map<std::string, int> &loginToUid) {

    std::cout << gid << " = " << group.first << ":";

    // check if there are no members, if so print no space
    if (!group.second.empty()) std::cout << " ";
    for (size_t i = 0; i < group.second.size(); ++i) {
        const auto &login = group.second[i];

        // find login and print
        auto it = loginToUid.find(login);
        std::cout << login;

        // print uid if known
        if (it != loginToUid.end()) {
            std::cout << "(" << it->second << ")";
        }

        if (i + 1 < group.second.size()) std::cout << " ";
    }
    std::cout << "\n";
}

// iterates through the vector inputs and prints each group
void printView(
    std::vector<std::string> &inputs,
    std::unordered_map<std::string, int> &loginToUid,
    std::unordered_map<int, std::string> &uidToLogin,
    std::unordered_map<int, std::pair<std::string,
        std::vector<std::string>>> &gidMap,
    std::unordered_map<std::string, int> &nameMap) {

    // if groups exist, print info
    for (auto &t : inputs) {
        if (isNumber(t)) {
            // if a valid number then and group found, print group
            int gid = std::stoi(t);
            if (gidMap.find(gid) != gidMap.end())
                printGroup(gid, gidMap[gid], loginToUid);
            else
                std::cout << t << " = Group not found.\n";
        } else {
            // if found print
            if (nameMap.find(t) != nameMap.end())
                printGroup(nameMap[t], gidMap[nameMap[t]], loginToUid);
            else
                // if not found print not found
                std::cout << t << " = Group not found.\n";
        }
    }
}

// prompts user to input names or gids and loads data from
// passwd and group files into their respective structures
void view() {
    auto inputs = getInput(
        "Enter groups to display (names or GIDs, space-delimited):");
    auto passwdLines = loadFile("passwd");
    auto groupLines = loadFile("group");

    std::unordered_map<std::string, int> loginToUid;
    std::unordered_map<int, std::string> uidToLogin;
    std::unordered_map<int, std::pair<std::string,
        std::vector<std::string>>> gidMap;
    std::unordered_map<std::string, int> nameMap;

    // fill maps with data from passwd and groups
    parsePasswd(passwdLines, loginToUid, uidToLogin);
    parseGroup(groupLines, gidMap, nameMap);

    printView(inputs, loginToUid, uidToLogin, gidMap, nameMap);
}

// ------- Delete functions -------

// checks if given gid is used as a primary group by another user
bool isPrimaryGroup(
    const std::string &gid,
    const std::vector<std::string> &passwd) {

    for (const auto &l : passwd) {
        std::istringstream iss(l);
        std::string userName, upass, uid, ugid;

        // get userName, upass, uid and ugid from the line
        if (!std::getline(iss, userName, ':')) continue;
        if (!std::getline(iss, upass, ':')) continue;
        if (!std::getline(iss, uid, ':')) continue;
        if (!std::getline(iss, ugid, ':')) continue;

        // if user's primary gid matches the given gid return true
        if (ugid == gid) return true;
    }
    return false;
}

// removes a group by name or gid if not primary
int removeGroup(
    const std::string &input,
    std::vector<std::string> &groups,
    const std::vector<std::string> &passwd) {

    for (auto it = groups.begin(); it != groups.end(); ++it) {
        std::istringstream iss(*it);
        std::string name, pass, gid;

        // get name, pass and gid from the line
        if (!std::getline(iss, name, ':')) continue;
        if (!std::getline(iss, pass, ':')) continue;
        if (!std::getline(iss, gid, ':')) continue;

        // if the current group matches gid or name
        if (name != input && gid != input) continue;

        // current group is primary for some user, cannot delete
        if (isPrimaryGroup(gid, passwd)) return 2;

        // erase from groups vector if not primary group
        groups.erase(it);
        return 1;
    }
    return 0;
}

// finds the groups index by gid, and names the group
int findGroup(
    const std::string &gid, const std::vector<std::string> &groups,
    std::string &groupName) {

    for (size_t i = 0; i < groups.size(); ++i) {
        std::istringstream iss(groups[i]);
        std::string name, pass, id;

        // get name, pass and id from line
        if (!std::getline(iss, name, ':')) continue;
        if (!std::getline(iss, pass, ':')) continue;
        if (!std::getline(iss, id, ':')) continue;

        // if gid matches set name and return index of
        if (id == gid) {
            groupName = name;
            return static_cast<int>(i);
        }
    }
    return -1;
}

// overwrites the group file with updated group list
void saveGroups(const std::vector<std::string> &groups) {
    std::ofstream ofs("group", std::ios::trunc);
    for (const auto &g : groups) {
        ofs << g << "\n";
    }
    ofs.close();
}

// deletes a group using user, repeating the prompt if
// the user gives an invalid gid
void del() {
    std::vector<std::string> groups = loadFile("group");
    std::vector<std::string> passwd = loadFile("passwd");

    while (true) {
        std::cout << "Enter GID to delete:\n";
        std::string gid;
        std::getline(std::cin, gid);

        // skip empty line
        if (gid.empty()) continue;
        std::string groupName;
        int index = findGroup(gid, groups, groupName);

        // if group does not exist, prompt user to enter a value again
        if (index == -1) {
            std::cout << "GID " << gid << " does not exist.\n";
            continue;
        }

        // primary group for some user, cannot delete
        if (isPrimaryGroup(gid, passwd)) {
            std::cout << "Cannot delete GID " << gid
                      << " (it is a primary group).  Try another.\n";
            continue;
        }

        // delete groups from vector and save back to file
        groups.erase(groups.begin() + index);
        saveGroups(groups);
        std::cout << "Deleted group '" << groupName
                  << "' (gid " << gid << ").\n";
        break;
    }
}

// ------- Add functions -------

// searches for the login name by uid in the passwd file
std::string getLoginFromUid(
    const std::string &uid,
    const std::vector<std::string> &passwd) {

    for (const auto &p : passwd) {
        std::istringstream iss(p);
        std::string usrName, pass, id;

        // get usrName, pass and id from line
        if (!std::getline(iss, usrName, ':')) continue;
        if (!std::getline(iss, pass, ':')) continue;
        if (!std::getline(iss, id, ':')) continue;
        if (id == uid) return usrName;
    }
    return "";
}

// checks if a group name or gid already exists
bool groupExists(
    const std::string &gid, const std::string &name,
    const std::vector<std::string> &groups) {

    for (const auto &g : groups) {
        std::istringstream iss(g);
        std::string groupName, pass, id;

        // get groupName, pass and id from line
        if (!std::getline(iss, groupName, ':')) continue;
        if (!std::getline(iss, pass, ':')) continue;
        if (!std::getline(iss, id, ':')) continue;

        // if a current gid already exists or if the name matches a
        // existing group then return true
        if ((!gid.empty() && id == gid) || (!name.empty() && groupName == name))
            return true;
    }
    return false;
}

// checks if a uid exists in the passwd file
bool uidExists(
    const std::string &uid,
    const std::vector<std::string> &passwd) {

    for (const auto &p : passwd) {
        std::istringstream iss(p);
        std::string userName, pass, id;

        // get userName, pass and id from line
        if (!std::getline(iss, userName, ':')) continue;
        if (!std::getline(iss, pass, ':')) continue;
        if (!std::getline(iss, id, ':')) continue;

        // return true if one exists
        if (id == uid) return true;
    }
    return false;
}

// prompts the user to enter a gid and will repeat if
// that gid already exists or is invalid
std::string promptNewGid(const std::vector<std::string> &groups) {
    std::string gid;

    while (true) {
        std::cout << "Enter new GID to add:\n";
        std::getline(std::cin, gid);

        // loop again if no gid entered
        if (gid.empty()) continue;

        // if the group exists, display error message and loop
        if (groupExists(gid, "", groups)) {
            std::cout << "GID " << gid << " already exists.  Try another.\n";
            continue;
        }
        break;
    }
    return gid;
}

// prompts the user for a name for their group
std::string promptNewName(const std::vector<std::string> &groups) {
    std::string name;
    while (true) {
        std::cout << "Enter new group name:\n";
        std::getline(std::cin, name);

        // loop again if no name entered
        if (name.empty()) continue;

        // if the group exists, display error message and loop
        if (groupExists("", name, groups)) {
            std::cout << "Group name is in use.  Choose another.\n";
            continue;
        }
        break;
    }
    return name;
}

// prompts the user for a uid or uids and validates each one exists
std::vector<std::string> promptUids(const std::vector<std::string> &passwd) {
    std::vector<std::string> uids;
    while (true) {
        std::cout << "Enter space-delimited list of UIDs to add:\n";
        std::string line;
        std::getline(std::cin, line);

        // loop again if no uid entered
        if (line.empty()) break;

        std::istringstream iss(line);
        std::string uid;
        bool uidTaken = false;
        while (iss >> uid) {
            // check if the uid already exists, if so set taken to true
            if (!uidExists(uid, passwd)) {
                std::cout << "UID " << uid
                          << " does not exist.  Please try again.\n";
                uidTaken = true;
                break;
            }
            uids.push_back(uid);
        }
        // break loop if valid uid
        if (!uidTaken) break;
        uids.clear();
    }

    // returns a list of uids entered by user
    return uids;
}

// this method creates and saves a new group
void add() {
    std::vector<std::string> groups = loadFile("group");
    std::vector<std::string> passwd = loadFile("passwd");

    // get group info from user
    std::string gid = promptNewGid(groups);
    std::string name = promptNewName(groups);
    std::vector<std::string> uids = promptUids(passwd);

    // convert uids to usernames
    std::vector<std::string> logins;
    for (const auto &uid : uids) {
        std::string login = getLoginFromUid(uid, passwd);
        if (!login.empty()) logins.push_back(login);
    }

    // format group entry for file
    std::ostringstream entry;
    entry << name << ":x:" << gid << ":";
    for (size_t i = 0; i < logins.size(); ++i) {
        if (i) entry << ",";
        entry << logins[i];
    }

    // save to file and print what was saved
    groups.push_back(entry.str());
    saveGroups(groups);
    std::cout << "Group '" << name << "' (gid " << gid << ") added with "
              << logins.size() << " member(s).\n";
}

// ------- MVP functions -------

// prints the usage if there is a input error
void printUsage() {
    std::cerr << "Usage: homework01 <v|d|a>\n"
              << "  v : view groups (prompt for names/GIDs)\n"
              << "  d : delete a group (cannot be a primary group)\n"
              << "  a : add a new group (gid, name, members by UIDs)\n";
}

// checks to make sure the program has the correct number of
// args and that the option selected is valid
bool checkCLA(int argc, char* argv[]) {
    if (argc != 2) {
        printUsage();
        return false;
    }
    if (!std::string(argv[1]).empty() &&
        (argv[1][0] != 'v' && argv[1][0] != 'd' &&
            argv[1][0] != 'a')) {
        printUsage();
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    // check for the correct number of arguments
    if (!checkCLA(argc, argv)) return 0;

    std::string selection = argv[1];
    if (selection == "v") view();
    else if (selection == "d") del();
    else if (selection == "a") add();

    return 0;
}
