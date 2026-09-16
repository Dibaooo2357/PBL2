#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <conio.h>
#include <filesystem>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\nNhan Enter de tiep tuc...";
    cin.ignore();
    cin.get();
}

string formatCurrency(double amount) {
    long long value = static_cast<long long>(amount);
    string str = to_string(value);
    int n = str.length();

    if (n <= 3) return str;

    string formatted = "";
    int count = 0;

    for (int i = n - 1; i >= 0; --i) {
        formatted += str[i];
        count++;
        if (count % 3 == 0 && i != 0) {
            formatted += '.';
        }
    }

    reverse(formatted.begin(), formatted.end());
    return formatted;
}

int navigateMenu(const string& title, const vector<string>& options) {
    int currentSelection = 0;
    int key;

    while (true) {
        clearScreen();
        cout << "================ " << title << " ================\n";
        cout << "   (Dung mui ten UP/DOWN de di chuyen, ENTER de chon)\n";
        cout << "----------------------------------------------------------------\n";

        for (size_t i = 0; i < options.size(); ++i) {
            if (static_cast<int>(i) == currentSelection) {
                cout << "  > " << options[i] << " <\n";
            } else {
                cout << "    " << options[i] << "\n";
            }
        }
        cout << "----------------------------------------------------------------\n";

        key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == KEY_UP) {
                currentSelection = (currentSelection - 1 + options.size()) % options.size();
            } else if (key == KEY_DOWN) {
                currentSelection = (currentSelection + 1) % options.size();
            }
        } else if (key == KEY_ENTER) {
            return currentSelection;
        }
    }
}

// ================= HE THONG QUAN LY TAI KHOAN & PHAN QUYEN =================
enum Role { KHANG_HANG, NHAN_VIEN, QUAN_LY, KHONG_XAC_DINH };

struct UserSession {
    string id;
    string fullName;
    string dob;
    string phone;
    Role role = KHONG_XAC_DINH;
    bool isLoggedIn = false;
};

class AccountManager {
public:
    static void createAccount(const UserSession& currentUser) {
        clearScreen();
        cout << "================ TAO TAI KHOAN MOI ================\n";

        if (!currentUser.isLoggedIn) {
            cout << "=> Loi: Ban can dang nhap de thuc hien chuc nang nay!\n";
            pauseScreen();
            return;
        }

        if (currentUser.role == KHANG_HANG) {
            cout << "=> Loi: Khach hang khong co quyen tao tai khoan!\n";
            pauseScreen();
            return;
        }

        string id, password, fullName, dob, phone, folder;

        if (currentUser.role == NHAN_VIEN) {
            cout << "[Quyen Nhan Vien]: Ban chi duoc phep tao tai khoan KHACH HANG (Prefix: KH_)\n";
            cout << "Nhap ID (vi du: KH_001): ";
            cin >> id;

            if (id.length() < 3 || id.substr(0, 3) != "KH_") {
                cout << "=> Loi: Nhan vien chi duoc tao ID bat dau bang 'KH_'!\n";
                pauseScreen();
                return;
            }
            folder = "DataKhachHang";
        }
        else if (currentUser.role == QUAN_LY) {
            cout << "[Quyen Quan Ly]: Ban duoc phep tao tai khoan NHAN VIEN (NV_) hoac KHACH HANG (KH_)\n";
            cout << "Nhap ID (vi du: NV_001 hoac KH_001): ";
            cin >> id;

            if (id.length() >= 3 && id.substr(0, 3) == "NV_") {
                folder = "DataNhanVien";
            } else if (id.length() >= 3 && id.substr(0, 3) == "KH_") {
                folder = "DataKhachHang";
            } else {
                cout << "=> Loi: ID phai bat dau bang 'NV_' hoac 'KH_'!\n";
                pauseScreen();
                return;
            }
        }

        filesystem::create_directories(folder);
        string filePath = folder + "/" + id + ".txt";

        if (filesystem::exists(filePath)) {
            cout << "=> Loi: ID nay da ton tai trong he thong!\n";
            pauseScreen();
            return;
        }

        cout << "Nhap Mat khau: ";
        cin >> password;
        cin.ignore();

        cout << "Nhap Ho va Ten: ";
        getline(cin, fullName);

        cout << "Nhap Ngay thang nam sinh (dd/mm/yyyy): ";
        getline(cin, dob);

        cout << "Nhap So dien thoai: ";
        getline(cin, phone);

        ofstream file(filePath);
        if (!file) {
            cout << "=> Loi: Khong the tao file du lieu!\n";
            pauseScreen();
            return;
        }

        file << id << endl;
        file << password << endl;
        file << fullName << endl;
        file << dob << endl;
        file << phone << endl;
        file.close();

        cout << "\n=> Da tao tai khoan " << id << " (" << fullName << ") thanh cong!\n";
        pauseScreen();
    }

    static bool login(UserSession& session) {
        clearScreen();
        cout << "================ DANG NHAP HE THONG ================\n";
        cout << "Nhap ID: ";
        cin >> session.id;

        string folder;
        if (session.id.length() >= 3) {
            string prefix = session.id.substr(0, 3);
            if (prefix == "KH_") {
                folder = "DataKhachHang";
                session.role = KHANG_HANG;
            } else if (prefix == "NV_") {
                folder = "DataNhanVien";
                session.role = NHAN_VIEN;
            } else if (prefix == "QL_") {
                folder = "DataQuanLi";
                session.role = QUAN_LY;
            }
        }

        if (folder.empty()) {
            cout << "=> Loi: ID khong hop le (Phai bat dau bang KH_, NV_ hoac QL_)!\n";
            session.role = KHONG_XAC_DINH;
            pauseScreen();
            return false;
        }

        string filePath = folder + "/" + session.id + ".txt";
        if (!filesystem::exists(filePath)) {
            cout << "=> Loi: Tai khoan khong ton tai!\n";
            session.role = KHONG_XAC_DINH;
            pauseScreen();
            return false;
        }

        string inputPass, fileId, filePass, fullName, dob, phone;
        cout << "Nhap Mat khau: ";
        cin >> inputPass;

        ifstream file(filePath);
        if (getline(file, fileId) && getline(file, filePass)) {
            if (inputPass == filePass) {
                getline(file, fullName);
                getline(file, dob);
                getline(file, phone);

                session.fullName = fullName.empty() ? "N/A" : fullName;
                session.dob = dob.empty() ? "N/A" : dob;
                session.phone = phone.empty() ? "N/A" : phone;
                session.isLoggedIn = true;

                cout << "\n=> Dang nhap thanh cong!\n";
                cout << "   Xin chao: " << session.fullName << "\n";
                cout << "   Ngay sinh: " << session.dob << " | SDT: " << session.phone << "\n";
                cout << "   Vai tro: ";
                if (session.role == KHANG_HANG) cout << "KHACH HANG\n";
                else if (session.role == NHAN_VIEN) cout << "NHAN VIEN\n";
                else if (session.role == QUAN_LY) cout << "QUAN LY\n";

                pauseScreen();
                return true;
            }
        }

        cout << "=> Loi: Mat khau khong chinh xac!\n";
        session.role = KHONG_XAC_DINH;
        pauseScreen();
        return false;
    }

    static void displayAccountList(const string& folderName, const string& title) {
        clearScreen();
        cout << "================ DANH SACH " << title << " ================\n";
        
        if (!filesystem::exists(folderName) || filesystem::is_empty(folderName)) {
            cout << "=> Hien tai chua co " << title << " nao trong he thong!\n";
            pauseScreen();
            return;
        }

        cout << "-----------------------------------------------------------------------------------------\n";
        cout << "| " << left << setw(6) << "STT" 
             << "| " << setw(12) << "ID" 
             << "| " << setw(25) << "Ho va Ten" 
             << "| " << setw(15) << "Ngay sinh" 
             << "| " << setw(15) << "So dien thoai" << " |\n";
        cout << "-----------------------------------------------------------------------------------------\n";

        int count = 0;
        for (const auto& entry : filesystem::directory_iterator(folderName)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                count++;
                ifstream file(entry.path());
                string id, pass, fullName, dob, phone;

                getline(file, id);
                getline(file, pass);
                getline(file, fullName);
                getline(file, dob);
                getline(file, phone);

                if (fullName.empty()) fullName = "N/A";
                if (dob.empty()) dob = "N/A";
                if (phone.empty()) phone = "N/A";

                cout << "| " << left << setw(6) << count 
                     << "| " << setw(12) << id 
                     << "| " << setw(25) << fullName 
                     << "| " << setw(15) << dob 
                     << "| " << setw(15) << phone << " |\n";
            }
        }

        cout << "-----------------------------------------------------------------------------------------\n";
        cout << "Tong so luong: " << count << " tai khoan.\n";
        pauseScreen();
    }

    static void menuDisplayAccounts(Role role) {
        if (role == QUAN_LY) {
            vector<string> options = {
                "1. Danh sach NHAN VIEN",
                "2. Danh sach KHACH HANG",
                "0. Quay lai"
            };
            int choice = navigateMenu("XUAT DANH SACH TAI KHOAN", options);
            if (choice == 0) displayAccountList("DataNhanVien", "NHAN VIEN");
            else if (choice == 1) displayAccountList("DataKhachHang", "KHACH HANG");
        } else if (role == NHAN_VIEN) {
            displayAccountList("DataKhachHang", "KHACH HANG");
        }
    }

    static void deleteAccount(const UserSession& currentUser, const string& folderName, const string& title) {
        clearScreen();
        cout << "================ XOA TAI KHOAN " << title << " ================\n";

        if (!filesystem::exists(folderName) || filesystem::is_empty(folderName)) {
            cout << "=> Hien tai chua co " << title << " nao trong he thong de xoa!\n";
            pauseScreen();
            return;
        }

        string idToDelete;
        cout << "Nhap ID " << title << " can xoa: ";
        cin >> idToDelete;

        string filePath = folderName + "/" + idToDelete + ".txt";

        if (!filesystem::exists(filePath)) {
            cout << "=> Loi: Khong tim thay tai khoan voi ID '" << idToDelete << "' trong danh sach!\n";
            pauseScreen();
            return;
        }

        cout << "Ban co chac chan muon xoa tai khoan " << idToDelete << " hay khong? (Y/N): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'Y' || confirm == 'y') {
            if (filesystem::remove(filePath)) {
                cout << "=> Da xoa thanh cong tai khoan " << idToDelete << "!\n";
            } else {
                cout << "=> Loi: Khong the xoa file du lieu cua tai khoan nay!\n";
            }
        } else {
            cout << "=> Da huy thao tac xoa.\n";
        }

        pauseScreen();
    }

    static void menuDeleteAccount(Role role) {
        if (role == QUAN_LY) {
            vector<string> options = {
                "1. Xoa tai khoan NHAN VIEN",
                "2. Xoa tai khoan KHACH HANG",
                "0. Quay lai"
            };
            int choice = navigateMenu("XOA TAI KHOAN", options);
            UserSession dummySession;
            if (choice == 0) deleteAccount(dummySession, "DataNhanVien", "NHAN VIEN");
            else if (choice == 1) deleteAccount(dummySession, "DataKhachHang", "KHACH HANG");
        } else if (role == NHAN_VIEN) {
            UserSession dummySession;
            deleteAccount(dummySession, "DataKhachHang", "KHACH HANG");
        }
    }
};

// ================= HE THONG QUAN LY KHO HANG =================
struct PhoneModel {
    int modelId;
    string name;
    double importPrice;
    double sellingPrice;
};

struct PhoneItem {
    string serialNumber;
    int modelId;
    int importOrder;
};

class InventoryManager {
private:
    vector<PhoneModel> models;
    vector<PhoneItem> items;
    int autoModelId = 1;
    int globalImportOrder = 1;
    const string inventoryFolder = "DataKhoHang";
    const string inventoryFile = "DataKhoHang/KhoHang.txt";

    int findModelByName(const string& name) {
        for (const auto& m : models) {
            if (m.name == name) return m.modelId;
        }
        return -1;
    }

    PhoneModel getModelById(int id) const {
        for (const auto& m : models) {
            if (m.modelId == id) return m;
        }
        return {-1, "Unknown", 0.0, 0.0};
    }

    int getOrCreateModel(const string& name, double importPrice, double sellingPrice) {
        int modelId = findModelByName(name);
        if (modelId == -1) {
            PhoneModel newModel = {autoModelId, name, importPrice, sellingPrice};
            models.push_back(newModel);
            modelId = autoModelId++;
        }
        return modelId;
    }

    int countStockByModelId(int modelId) const {
        int count = 0;
        for (const auto& item : items) {
            if (item.modelId == modelId) count++;
        }
        return count;
    }

public:
    InventoryManager() {
        filesystem::create_directories(inventoryFolder);
        loadInventoryFromFile();
    }

    void saveInventoryToFile() {
        ofstream file(inventoryFile);
        if (!file.is_open()) return;

        for (const auto& item : items) {
            PhoneModel m = getModelById(item.modelId);
            file << m.name << "," 
                 << item.serialNumber << "," 
                 << static_cast<long long>(m.importPrice) << "," 
                 << static_cast<long long>(m.sellingPrice) << "\n";
        }
        file.close();
    }

    void loadInventoryFromFile() {
        if (!filesystem::exists(inventoryFile)) return;

        ifstream file(inventoryFile);
        if (!file.is_open()) return;

        models.clear();
        items.clear();
        autoModelId = 1;
        globalImportOrder = 1;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string name, serial, strImportPrice, strSellingPrice;

            if (getline(ss, name, ',') &&
                getline(ss, serial, ',') &&
                getline(ss, strImportPrice, ',') &&
                getline(ss, strSellingPrice, ',')) {

                double importPrice = stod(strImportPrice);
                double sellingPrice = stod(strSellingPrice);

                int modelId = getOrCreateModel(name, importPrice, sellingPrice);
                items.push_back({serial, modelId, globalImportOrder++});
            }
        }
        file.close();
    }

    void inputItemManual() {
        clearScreen();
        string name, serial;
        double importPrice = 0, sellingPrice = 0;

        cout << "================ NHAP THU CONG MOT THIET BI ================\n";
        cout << "Nhap Ten mat hang: ";
        cin.ignore();
        getline(cin, name);

        int modelId = findModelByName(name);
        if (modelId == -1) {
            cout << "=> Mat hang moi! Nhap thong tin gia:\n";
            cout << "   Gia nhap: "; cin >> importPrice;
            cout << "   Gia ban: "; cin >> sellingPrice;
            cin.ignore();
            modelId = getOrCreateModel(name, importPrice, sellingPrice);
        } else {
            cout << "=> Mat hang da ton tai. Dung lai thong tin gia cu.\n";
            cin.ignore();
        }

        cout << "Nhap So Seri: ";
        getline(cin, serial);

        items.push_back({serial, modelId, globalImportOrder++});
        saveInventoryToFile();

        cout << "\n=> Da nhap va luu du lieu thanh cong (Seri: " << serial << ")!\n";
        pauseScreen();
    }

    void inputItemsFromFile() {
        vector<string> availableFiles;

        DIR* dir = opendir(".");
        if (dir != NULL) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                string filename = entry->d_name;
                if (filename.length() >= 4) {
                    string ext = filename.substr(filename.length() - 4);
                    if (ext == ".txt" || ext == ".csv") {
                        if (filename != "KhoHang.txt") {
                            availableFiles.push_back(filename);
                        }
                    }
                }
            }
            closedir(dir);
        } else {
            clearScreen();
            cout << "=> Loi: Khong the mo thu muc hien tai!\n";
            pauseScreen();
            return;
        }

        if (availableFiles.empty()) {
            clearScreen();
            cout << "=> Khong tim thay file .txt hoac .csv nao trong thu muc!\n";
            pauseScreen();
            return;
        }

        vector<string> menuOptions = availableFiles;
        menuOptions.push_back("Chon CAC FILE CAN NHAP (Nhap danh sach)");
        menuOptions.push_back("Chon TAT CA (Nhap toan bo file)");
        menuOptions.push_back("Quay lai menu chinh");

        int choice = navigateMenu("DANH SACH FILE DU LIEU CO SAN", menuOptions);

        vector<string> filesToProcess;
        int totalFiles = availableFiles.size();

        if (choice == totalFiles + 2) return;
        else if (choice == totalFiles + 1) filesToProcess = availableFiles;
        else if (choice == totalFiles) {
            clearScreen();
            cout << "================ NHAP DANH SACH FILE DU LIEU ================\n";
            for (size_t i = 0; i < availableFiles.size(); ++i) {
                cout << i + 1 << ". " << availableFiles[i] << "\n";
            }
            cout << "----------------------------------------------------------------\n";
            cout << "Nhap cac so thu tu file (cach nhau bang dau cach hoac phay): ";
            cin.ignore();
            string inputStr;
            getline(cin, inputStr);

            for (char &c : inputStr) {
                if (c == ',') c = ' ';
            }

            stringstream ssInput(inputStr);
            int fileIndex;
            while (ssInput >> fileIndex) {
                if (fileIndex >= 1 && fileIndex <= static_cast<int>(availableFiles.size())) {
                    filesToProcess.push_back(availableFiles[fileIndex - 1]);
                }
            }

            if (filesToProcess.empty()) {
                cout << "=> Khong co file hop le nao duoc chon!\n";
                pauseScreen();
                return;
            }
        } 
        else {
            filesToProcess.push_back(availableFiles[choice]);
        }

        clearScreen();
        int totalSuccess = 0;
        cout << "================ KET QUA NHAP DU LIEU ================\n";

        for (const string& selectedFilename : filesToProcess) {
            ifstream file(selectedFilename);

            if (!file.is_open()) {
                cout << "=> Loi: Khong the mo file '" << selectedFilename << "'!\n";
                continue;
            }

            string line;
            int countSuccessInFile = 0;

            while (getline(file, line)) {
                if (line.empty()) continue;

                stringstream ss(line);
                string name, serial, strImportPrice, strSellingPrice;

                if (getline(ss, name, ',') &&
                    getline(ss, serial, ',') &&
                    getline(ss, strImportPrice, ',') &&
                    getline(ss, strSellingPrice, ',')) {

                    double importPrice = stod(strImportPrice);
                    double sellingPrice = stod(strSellingPrice);

                    int modelId = getOrCreateModel(name, importPrice, sellingPrice);
                    items.push_back({serial, modelId, globalImportOrder++});
                    countSuccessInFile++;
                }
            }

            file.close();
            totalSuccess += countSuccessInFile;
            cout << "=> Da doc file '" << selectedFilename << "': " << countSuccessInFile << " thiet bi.\n";
        }

        saveInventoryToFile();
        cout << "----------------------------------------------------------------\n";
        cout << "=> TONG CONG: Da nhap va luu thanh cong " << totalSuccess << " thiet bi vao Kho Hang!\n";
        pauseScreen();
    }

    void menuInputItems() {
        vector<string> options = {
            "1. Nhap hang thu cong (1 thiet bi)",
            "2. Nhap hang HANG LOAT bang FILE",
            "0. Quay lai"
        };
        int choice = navigateMenu("NHAP HANG VAO KHO", options);
        if (choice == 0) inputItemManual();
        else if (choice == 1) inputItemsFromFile();
    }

    void displayItems(Role currentRole) {
        if (items.empty()) {
            clearScreen();
            cout << "\n=> Kho hang hien dang rong!\n";
            pauseScreen();
            return;
        }

        vector<string> displayOptions;
        if (currentRole == KHANG_HANG) {
            displayOptions = {
                "Gia ban tu CAO den THAP",
                "Gia ban tu THAP den CAO",
                "Quay lai"
            };
        } else {
            displayOptions = {
                "Gia ban tu CAO den THAP",
                "Gia ban tu THAP den CAO",
                "Mat hang MOI nhap (Moi nhat len tren)",
                "Mat hang CU (Nhap dau tien len tren)",
                "Quay lai"
            };
        }

        int choice = navigateMenu("TUY CHON XUAT DANH SACH KHO HANG", displayOptions);
        
        if (currentRole == KHANG_HANG && choice == 2) return;
        if (currentRole != KHANG_HANG && choice == 4) return;

        vector<PhoneItem> sortedItems = items;

        switch (choice) {
            case 0:
                sort(sortedItems.begin(), sortedItems.end(), [this](const PhoneItem& a, const PhoneItem& b) {
                    return getModelById(a.modelId).sellingPrice > getModelById(b.modelId).sellingPrice;
                });
                break;
            case 1:
                sort(sortedItems.begin(), sortedItems.end(), [this](const PhoneItem& a, const PhoneItem& b) {
                    return getModelById(a.modelId).sellingPrice < getModelById(b.modelId).sellingPrice;
                });
                break;
            case 2:
                sort(sortedItems.begin(), sortedItems.end(), [](const PhoneItem& a, const PhoneItem& b) {
                    return a.importOrder > b.importOrder;
                });
                break;
            case 3:
                sort(sortedItems.begin(), sortedItems.end(), [](const PhoneItem& a, const PhoneItem& b) {
                    return a.importOrder < b.importOrder;
                });
                break;
        }

        clearScreen();
        cout << "================ DANH SACH DANH MUC KHO HANG ================\n";
        
        if (currentRole == KHANG_HANG) {
            cout << "------------------------------------------------------------------------\n";
            cout << "| " << left << setw(28) << "Ten mat hang"
                 << "| " << setw(22) << "So Seri"
                 << "| " << setw(18) << "Gia ban (VND)" << " |\n";
            cout << "------------------------------------------------------------------------\n";

            for (const auto& item : sortedItems) {
                PhoneModel m = getModelById(item.modelId);
                cout << "| " << left << setw(28) << m.name
                     << "| " << setw(22) << item.serialNumber
                     << "| " << setw(18) << formatCurrency(m.sellingPrice) << " |\n";
            }
            cout << "------------------------------------------------------------------------\n";
        } else {
            cout << "------------------------------------------------------------------------------------\n";
            cout << "| " << left << setw(22) << "Ten mat hang"
                 << "| " << setw(18) << "So Seri"
                 << "| " << setw(15) << "Gia nhap (VND)"
                 << "| " << setw(15) << "Gia ban (VND)" << " |\n";
            cout << "------------------------------------------------------------------------------------\n";

            for (const auto& item : sortedItems) {
                PhoneModel m = getModelById(item.modelId);
                cout << "| " << left << setw(22) << m.name
                     << "| " << setw(18) << item.serialNumber
                     << "| " << setw(15) << formatCurrency(m.importPrice)
                     << "| " << setw(15) << formatCurrency(m.sellingPrice) << " |\n";
            }
            cout << "------------------------------------------------------------------------------------\n";
        }
        
        pauseScreen();
    }

    void adviseCustomer() {
        clearScreen();
        cout << "================ TU VAN MUA DIEN THOAI ================\n";
        if (items.empty()) {
            cout << "=> Kho hang hien dang rong, khong the tu van!\n";
            pauseScreen();
            return;
        }

        double budget;
        cout << "Nhap so tien hien co (VND): ";
        cin >> budget;

        vector<PhoneModel> affordableModels;
        for (const auto& m : models) {
            if (m.sellingPrice <= budget && countStockByModelId(m.modelId) > 0) {
                affordableModels.push_back(m);
            }
        }

        if (affordableModels.empty()) {
            cout << "\n=> Rat tiec, khong co san pham nao phu hop voi ngan sach "
                 << formatCurrency(budget) << " VND!\n";
            pauseScreen();
            return;
        }

        sort(affordableModels.begin(), affordableModels.end(), [](const PhoneModel& a, const PhoneModel& b) {
            return a.sellingPrice < b.sellingPrice;
        });

        clearScreen();
        cout << "================ KET QUA TU VAN BAN HANG ================\n";
        cout << "Ngan sach: " << formatCurrency(budget) << " VND\n";
        cout << "-------------------------------------------------------------------------------------\n";
        cout << "| " << left << setw(18) << "Tieu chi"
             << "| " << setw(24) << "Ten mat hang"
             << "| " << setw(16) << "Gia ban (VND)"
             << "| " << setw(16) << "Tiet kiem (VND)" << " |\n";
        cout << "-------------------------------------------------------------------------------------\n";

        PhoneModel budgetOpt = affordableModels.front();
        cout << "| " << left << setw(18) << "1. Toi uu chi phi"
             << "| " << setw(24) << budgetOpt.name
             << "| " << setw(16) << formatCurrency(budgetOpt.sellingPrice)
             << "| " << setw(16) << formatCurrency(budget - budgetOpt.sellingPrice) << " |\n";

        if (affordableModels.size() >= 3) {
            int midIndex = affordableModels.size() / 2;
            PhoneModel midOpt = affordableModels[midIndex];
            cout << "| " << left << setw(18) << "2. Trung binh"
                 << "| " << setw(24) << midOpt.name
                 << "| " << setw(16) << formatCurrency(midOpt.sellingPrice)
                 << "| " << setw(16) << formatCurrency(budget - midOpt.sellingPrice) << " |\n";
        }

        if (affordableModels.size() >= 2) {
            PhoneModel bestOpt = affordableModels.back();
            string optName = (affordableModels.size() == 2) ? "2. Tot nhat" : "3. Tot nhat";
            cout << "| " << left << setw(18) << optName
                 << "| " << setw(24) << bestOpt.name
                 << "| " << setw(16) << formatCurrency(bestOpt.sellingPrice)
                 << "| " << setw(16) << formatCurrency(budget - bestOpt.sellingPrice) << " |\n";
        }

        cout << "-------------------------------------------------------------------------------------\n";
        pauseScreen();
    }
};

int main() {
    filesystem::create_directories("DataKhachHang");
    filesystem::create_directories("DataNhanVien");
    filesystem::create_directories("DataQuanLi");
    filesystem::create_directories("DataKhoHang");

    if (!filesystem::exists("DataQuanLi/QL_admin.txt")) {
        ofstream adminFile("DataQuanLi/QL_admin.txt");
        adminFile << "QL_admin\nadmin123\nQuan Ly Mien Nam\n01/01/1990\n0905123456\n";
        adminFile.close();
    }

    UserSession currentUser;
    InventoryManager manager;

    while (true) {
        if (!currentUser.isLoggedIn) {
            vector<string> loginOptions = {
                "1. Dang nhap",
                "0. Thoat"
            };
            int choice = navigateMenu("HE THONG CHUA DANG NHAP", loginOptions);
            if (choice == 0) {
                AccountManager::login(currentUser);
            } else {
                break;
            }
        } else {
            string title = "HE THONG QUAN LY (User: " + currentUser.id + " - " + currentUser.fullName + ")";
            vector<string> mainMenuOptions;

            if (currentUser.role == QUAN_LY) {
                mainMenuOptions = {
                    "1. Tao tai khoan moi",
                    "2. Nhap hang vao kho",
                    "3. Xuat danh sach kho hang",
                    "4. Xuat danh sach tai khoan",
                    "5. Xoa tai khoan",
                    "6. Tu van ban hang",
                    "7. Dang xuat",
                    "0. Thoat"
                };
            } else if (currentUser.role == NHAN_VIEN) {
                mainMenuOptions = {
                    "1. Tao tai khoan KHACH HANG",
                    "2. Xuat danh sach kho hang",
                    "3. Xuat danh sach KHACH HANG",
                    "4. Xoa tai khoan KHACH HANG",
                    "5. Tu van ban hang",
                    "6. Dang xuat",
                    "0. Thoat"
                };
            } else if (currentUser.role == KHANG_HANG) {
                mainMenuOptions = {
                    "1. Xem danh sach kho hang (Theo gia)",
                    "2. Tu van mua dien thoai",
                    "3. Dang xuat",
                    "0. Thoat"
                };
            }

            int choice = navigateMenu(title, mainMenuOptions);

            if (currentUser.role == QUAN_LY) {
                switch (choice) {
                    case 0: AccountManager::createAccount(currentUser); break;
                    case 1: manager.menuInputItems(); break;
                    case 2: manager.displayItems(currentUser.role); break;
                    case 3: AccountManager::menuDisplayAccounts(currentUser.role); break;
                    case 4: AccountManager::menuDeleteAccount(currentUser.role); break;
                    case 5: manager.adviseCustomer(); break;
                    case 6: currentUser = UserSession(); break;
                    case 7: 
                        clearScreen();
                        cout << "Tam biet!\n";
                        return 0;
                }
            } else if (currentUser.role == NHAN_VIEN) {
                switch (choice) {
                    case 0: AccountManager::createAccount(currentUser); break;
                    case 1: manager.displayItems(currentUser.role); break;
                    case 2: AccountManager::menuDisplayAccounts(currentUser.role); break;
                    case 3: AccountManager::menuDeleteAccount(currentUser.role); break;
                    case 4: manager.adviseCustomer(); break;
                    case 5: currentUser = UserSession(); break;
                    case 6: 
                        clearScreen();
                        cout << "Tam biet!\n";
                        return 0;
                }
            } else if (currentUser.role == KHANG_HANG) {
                switch (choice) {
                    case 0: manager.displayItems(currentUser.role); break;
                    case 1: manager.adviseCustomer(); break;
                    case 2: currentUser = UserSession(); break;
                    case 3: 
                        clearScreen();
                        cout << "Tam biet!\n";
                        return 0;
                }
            }
        }
    }

    return 0;
}