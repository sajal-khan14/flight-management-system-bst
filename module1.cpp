#include <iostream>
#include <string>
#include <fstream>

using namespace std;

enum RelationType { GREATER, LESS, EQUAL };

class ItemType
{
public:
    int id;
    string source, destination, airline;
    string departureTime, arrivalTime;
    string duration;            // stored as "H:MM/decimal", e.g. 1:45/1.75
    int ticketPrice;
    string status;

    RelationType ComparedTo(ItemType item)
    {
        if (id > item.id)
            return GREATER;
        else if (id < item.id)
            return LESS;
        else
            return EQUAL;
    }

    void Print()
    {
        cout << endl << "-----------------------------";
        cout << endl << "Flight ID: " << id;
        cout << endl << "Source: " << source;
        cout << endl << "Destination: " << destination;
        cout << endl << "Airline: " << airline;
        cout << endl << "Departure Time: " << departureTime;
        cout << endl << "Arrival Time: " << arrivalTime;
        cout << endl << "Duration: " << duration << " (H:MM / hours)";
        cout << endl << "Ticket Price: PKR " << ticketPrice;
        cout << endl << "Status: " << status;
    }
};

struct node
{
    ItemType data;
    node* left;
    node* right;
};

/* ---------------- Helper functions ---------------- */

string ToLower(string text)
{
    for (int i = 0; i < text.length(); i++)
    {
        if (text[i] >= 'A' && text[i] <= 'Z')
            text[i] = text[i] + 32;
    }
    return text;
}

bool IsDigits(string text)
{
    if (text.empty())
        return false;
    for (int i = 0; i < text.length(); i++)
    {
        if (text[i] < '0' || text[i] > '9')
            return false;
    }
    return true;
}

// Converts text to int safely. Returns false if text is not a whole number.
bool TryInt(string text, int& value)
{
    if (!IsDigits(text) || text.length() > 9)
        return false;
    value = stoi(text);
    return true;
}

// Checks HH:MM (24 hour)
bool IsValidTime(string t)
{
    if (t.length() != 5 || t[2] != ':')
        return false;
    if (!IsDigits(t.substr(0, 2)) || !IsDigits(t.substr(3, 2)))
        return false;
    return stoi(t.substr(0, 2)) < 24 && stoi(t.substr(3, 2)) < 60;
}

int ToMinutes(string t)
{
    return stoi(t.substr(0, 2)) * 60 + stoi(t.substr(3, 2));
}

// Accepts "1:45", "1.75" or "1:45/1.75" and converts to "1:45/1.75".
bool NormalizeDuration(string input, string& output)
{
    int slash = input.find('/');
    if (slash != string::npos)
        input = input.substr(0, slash);
    int totalMinutes = 0;
    int colon = input.find(':');
    if (colon != string::npos)
    {
        string h = input.substr(0, colon);
        string m = input.substr(colon + 1);
        if (!IsDigits(h) || !IsDigits(m) || h.length() > 3 || m.length() > 2)
            return false;
        if (stoi(m) >= 60)
            return false;
        totalMinutes = stoi(h) * 60 + stoi(m);
    }
    else
    {
        int dot = input.find('.');
        string whole = input.substr(0, dot);
        string frac = "0";
        if (dot != string::npos)
            frac = input.substr(dot + 1);
        if (!IsDigits(whole) || !IsDigits(frac) || whole.length() > 3
            || frac.length() > 4)
            return false;
        double hours = stod(input);
        totalMinutes = (int)(hours * 60 + 0.5);
    }
    if (totalMinutes <= 0)
        return false;
    int h = totalMinutes / 60;
    int m = totalMinutes % 60;
    // decimal hours rounded to 2 places, built with integers
    int hundredths = (totalMinutes * 100 + 30) / 60;
    output = to_string(h) + ":";
    if (m < 10)
        output += "0";
    output += to_string(m) + "/" + to_string(hundredths / 100) + ".";
    if (hundredths % 100 < 10)
        output += "0";
    output += to_string(hundredths % 100);
    return true;
}

// ETA = Departure Time + Flight Duration (wraps past midnight)
string CalculateETA(string departure, string duration)
{
    int hour = (departure[0] - '0') * 10 +
        (departure[1] - '0');
    int minute = (departure[3] - '0') * 10 +
        (departure[4] - '0');
    int colon = duration.find(':');
    int slash = duration.find('/');
    int durHours = stoi(duration.substr(0, colon));
    int durMinutes = stoi(duration.substr(colon + 1, slash - colon - 1));
    int total = hour * 60 + minute +
        durHours * 60 + durMinutes;
    hour = (total / 60) % 24;
    minute = total % 60;
    string result = "";
    if (hour < 10)
        result += "0";
    result += to_string(hour) + ":";
    if (minute < 10)
        result += "0";
    result += to_string(minute);
    return result;
}

/* ---------------- Input helpers ---------------- */

int ReadInt(string message)
{
    string line;
    int value;
    while (true)
    {
        cout << endl << message;
        getline(cin, line);
        if (TryInt(line, value))
            return value;
        cout << "Invalid number! Please enter digits only.";
    }
}

// Reads text; rejects empty text and commas (they would break flights.txt)
string ReadField(string message)
{
    string line;
    while (true)
    {
        cout << message;
        getline(cin, line);
        if (line.empty())
            cout << "Field cannot be empty!" << endl;
        else if (line.find(',') != string::npos)
            cout << "Commas are not allowed!" << endl;
        else
            return line;
    }
}

string ReadTime(string message)
{
    string line;
    while (true)
    {
        cout << message;
        getline(cin, line);
        if (IsValidTime(line))
            return line;
        cout << "Invalid time! Use 24-hour HH:MM (example 09:30)." << endl;
    }
}

string ReadDuration(string message)
{
    string line, result;
    while (true)
    {
        cout << message;
        getline(cin, line);
        if (NormalizeDuration(line, result))
            return result;
        cout << "Invalid duration! Use H:MM (1:45) or hours (1.75)." << endl;
    }
}

int ReadPrice(string message)
{
    string line;
    int value;
    while (true)
    {
        cout << message;
        getline(cin, line);
        if (TryInt(line, value))
            return value;
        cout << "Invalid price! Enter a whole number (PKR)." << endl;
    }
}

// Splits one file line into exactly 9 comma-separated fields
bool SplitLine(string line, string parts[])
{
    int start = 0;
    for (int i = 0; i < 8; i++)
    {
        int pos = line.find(',', start);
        if (pos == string::npos)
            return false;
        parts[i] = line.substr(start, pos - start);
        start = pos + 1;
    }
    parts[8] = line.substr(start);
    return parts[8].find(',') == string::npos;
}

string ReadStatus(string message)
{
    string line;
    while (true)
    {
        cout << message;
        getline(cin, line);
        string s = ToLower(line);
        if (s == "scheduled")
            return "Scheduled";
        if (s == "delayed")
            return "Delayed";
        if (s == "cancelled")
            return "Cancelled";
        cout << "Status must be Scheduled, Delayed or Cancelled." << endl;
    }
}

/* ---------------- Binary Search Tree ---------------- */

class BST
{
public:

    node* root;

    BST()
    {
        root = NULL;
    }

    ~BST()
    {
        Clear(root);
    }

    void Clear(node* temp)
    {
        if (temp != NULL)
        {
            Clear(temp->left);
            Clear(temp->right);
            delete temp;
        }
    }

    // Returns true if inserted, false if the Flight ID already exists
    bool Insert(ItemType item)
    {
        node* newNode = new node;
        newNode->data = item;
        newNode->left = NULL;
        newNode->right = NULL;
        if (root == NULL)
        {
            root = newNode;
            return true;
        }
        node* temp = root;
        node* parent = NULL;
        while (temp != NULL)
        {
            parent = temp;
            if (item.ComparedTo(temp->data) == LESS)
                temp = temp->left;
            else if (item.ComparedTo(temp->data) == GREATER)
                temp = temp->right;
            else
            {
                delete newNode;
                return false;
            }
        }
        if (item.ComparedTo(parent->data) == LESS)
            parent->left = newNode;
        else
            parent->right = newNode;
        return true;
    }

    void AddFlight(ItemType item)
    {
        if (Insert(item))
            cout << endl << "Flight added successfully!";
        else
            cout << endl << "Flight ID already exists! Duplicate rejected.";
    }

    node* Search(node* temp, int key)
    {
        if (temp == NULL || temp->data.id == key)
            return temp;
        if (key < temp->data.id)
            return Search(temp->left, key);
        return Search(temp->right, key);
    }

    void SearchFlight()
    {
        int id = ReadInt("Enter Flight ID: ");
        node* temp = Search(root, id);
        if (temp == NULL)
            cout << endl << "Flight not found!";
        else
            temp->data.Print();
    }

    void Inorder(node* temp)
    {
        if (temp != NULL)
        {
            Inorder(temp->left);
            temp->data.Print();
            Inorder(temp->right);
        }
    }

    void Display()
    {
        if (root == NULL)
            cout << endl << "No flights available!";
        else
            Inorder(root);
    }

    node* Minimum(node* temp)
    {
        while (temp->left != NULL)
            temp = temp->left;
        return temp;
    }

    node* DeleteNode(node* temp, int key)
    {
        if (temp == NULL)
            return temp;
        if (key < temp->data.id)
            temp->left = DeleteNode(temp->left, key);
        else if (key > temp->data.id)
            temp->right = DeleteNode(temp->right, key);
        else
        {
            if (temp->left == NULL)
            {
                node* child = temp->right;
                delete temp;
                return child;
            }
            else if (temp->right == NULL)
            {
                node* child = temp->left;
                delete temp;
                return child;
            }
            // two children: replace with inorder successor
            node* child = Minimum(temp->right);
            temp->data = child->data;
            temp->right = DeleteNode(temp->right,
                child->data.id);
        }
        return temp;
    }

    void DeleteFlight()
    {
        int id = ReadInt("Enter Flight ID to delete: ");
        if (Search(root, id) == NULL)
            cout << endl << "Flight not found!";
        else
        {
            root = DeleteNode(root, id);
            cout << endl << "Flight deleted successfully!";
        }
    }

    void ShowETA(node* temp)
    {
        if (temp == NULL)
            return;
        ShowETA(temp->left);
        string eta = CalculateETA(temp->data.departureTime,
            temp->data.duration);
        // difference in minutes, handling midnight wrap-around
        int diff = ToMinutes(eta) - ToMinutes(temp->data.arrivalTime);
        if (diff > 720)
            diff -= 1440;
        else if (diff < -720)
            diff += 1440;
        cout << endl << "-----------------------------";
        cout << endl << "Flight ID: " << temp->data.id;
        cout << endl << "Departure Time: " << temp->data.departureTime;
        cout << endl << "Flight Duration: " << temp->data.duration;
        cout << endl << "Calculated ETA: " << eta;
        cout << endl << "Scheduled Arrival: " << temp->data.arrivalTime;
        if (diff == 0)
            cout << endl << "Timing: On-Time";
        else if (diff < 0)
            cout << endl << "Timing: Early";
        else
            cout << endl << "Timing: Delayed";
        ShowETA(temp->right);
    }

    // Surprise feature: search all flights of one airline
    void SearchAirline(node* temp, string name, int& count)
    {
        if (temp != NULL)
        {
            SearchAirline(temp->left, name, count);
            if (ToLower(temp->data.airline) == name)
            {
                temp->data.Print();
                count++;
            }
            SearchAirline(temp->right, name, count);
        }
    }

    void AirlineFeature()
    {
        int count = 0;
        string name = ReadField("Enter Airline Name: ");
        SearchAirline(root, ToLower(name), count);
        if (count == 0)
            cout << endl << "No flights found for this airline!";
        else
            cout << endl << endl << "Total Flights Found: " << count;
    }

    // Preorder write: reloading rebuilds the same tree shape
    void WriteToFile(node* temp, ofstream& file)
    {
        if (temp != NULL)
        {
            file << temp->data.id << ","
                << temp->data.source << ","
                << temp->data.destination << ","
                << temp->data.airline << ","
                << temp->data.departureTime << ","
                << temp->data.arrivalTime << ","
                << temp->data.duration << ","
                << temp->data.ticketPrice << ","
                << temp->data.status << endl;
            WriteToFile(temp->left, file);
            WriteToFile(temp->right, file);
        }
    }

    void SaveToFile()
    {
        ofstream file("flights.txt");
        if (!file)
        {
            cout << endl << "File could not be opened!";
            return;
        }
        WriteToFile(root, file);
        file.close();
        cout << endl << "Flights saved successfully!";
    }

    void LoadFromFile()
    {
        ifstream file("flights.txt");
        if (!file)
        {
            cout << endl << "File not found!";
            return;
        }
        // clear old tree first so loading twice never creates duplicates
        Clear(root);
        root = NULL;
        string line;
        int loaded = 0, skipped = 0;
        while (getline(file, line))
        {
            if (!line.empty() && line[line.length() - 1] == '\r')
                line.erase(line.length() - 1);
            if (line.empty())
                continue;
            string p[9];
            ItemType item;
            bool ok = SplitLine(line, p);
            if (ok)
            {
                // price may be "18000" (or "18000.00" in older files)
                int dot = p[7].find('.');
                if (dot != string::npos)
                    p[7] = p[7].substr(0, dot);
                item.source = p[1];
                item.destination = p[2];
                item.airline = p[3];
                item.departureTime = p[4];
                item.arrivalTime = p[5];
                item.status = p[8];
                ok = TryInt(p[0], item.id) &&
                    TryInt(p[7], item.ticketPrice) &&
                    IsValidTime(item.departureTime) &&
                    IsValidTime(item.arrivalTime) &&
                    NormalizeDuration(p[6], item.duration);
            }
            if (ok && Insert(item))
                loaded++;
            else
                skipped++;
        }
        file.close();
        cout << endl << "File loaded: " << loaded << " flight(s) loaded";
        if (skipped > 0)
            cout << ", " << skipped << " invalid/duplicate line(s) skipped";
        cout << ".";
    }
};

int main()
{
    BST tree;
    tree.LoadFromFile();
    int choice;
    string line;
    do
    {
        cout << endl << endl << "========== FLIGHT MANAGEMENT SYSTEM ==========";
        cout << endl << "1. Add New Flight";
        cout << endl << "2. Search Flight by ID";
        cout << endl << "3. Delete Flight by ID";
        cout << endl << "4. Display All Flights (Ascending Flight ID)";
        cout << endl << "5. Calculate Flight Duration and ETA";
        cout << endl << "6. Surprise Feature (Search Flights by Airline)";
        cout << endl << "7. Save Flights to File";
        cout << endl << "8. Load Flights from File";
        cout << endl << "0. Exit";
        cout << endl << "==============================================";
        cout << endl << "Enter your choice: ";
        getline(cin, line);
        if (!TryInt(line, choice))
            choice = -1;
        if (choice == 1)
        {
            ItemType item;
            item.id = ReadInt("Enter Flight ID: ");
            item.source = ReadField("Enter Source: ");
            item.destination = ReadField("Enter Destination: ");
            item.airline = ReadField("Enter Airline: ");
            item.departureTime = ReadTime("Enter Departure Time (HH:MM): ");
            item.arrivalTime = ReadTime("Enter Arrival Time (HH:MM): ");
            item.duration = ReadDuration("Enter Duration (H:MM or hours, example 1:45): ");
            item.ticketPrice = ReadPrice("Enter Ticket Price (PKR): ");
            item.status = ReadStatus("Enter Status (Scheduled/Delayed/Cancelled): ");
            tree.AddFlight(item);
        }
        else if (choice == 2)
            tree.SearchFlight();
        else if (choice == 3)
            tree.DeleteFlight();
        else if (choice == 4)
            tree.Display();
        else if (choice == 5)
        {
            if (tree.root == NULL)
                cout << endl << "No flights available!";
            else
                tree.ShowETA(tree.root);
        }
        else if (choice == 6)
            tree.AirlineFeature();
        else if (choice == 7)
            tree.SaveToFile();
        else if (choice == 8)
            tree.LoadFromFile();
        else if (choice == 0)
            cout << endl << "Exiting Program...";
        else
            cout << endl << "Invalid Choice!";
    } while (choice != 0);
    return 0;
}

