#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <dirent.h>

using namespace std;

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

// 1. Struct lưu TÊN MẶT HÀNG & GIÁ
struct PhoneModel {
    int modelId;          // Mã định danh mặt hàng
    string name;          // Tên mặt hàng
    double importPrice;   // Giá nhập
    double sellingPrice;  // Giá bán
};

// 2. Struct lưu SỐ SERI & TRẠNG THÁI THIẾT BỊ
struct PhoneItem {
    string serialNumber;  // Số seri riêng của từng máy
    int modelId;          // Mã mặt hàng liên kết
    int importOrder;      // Thứ tự nhập (dùng để xác định mới/cũ)
};

class InventoryManager {
private:
    vector<PhoneModel> models;
    vector<PhoneItem> items;
    int autoModelId = 1;
    int globalImportOrder = 1;

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

    // Đếm số lượng tồn kho của 1 Model ID
    int countStockByModelId(int modelId) const {
        int count = 0;
        for (const auto& item : items) {
            if (item.modelId == modelId) count++;
        }
        return count;
    }

public:
    // 1. Nhập hàng thủ công
    void inputItemManual() {
        clearScreen();
        string name, serial;
        double importPrice = 0, sellingPrice = 0;

        cout << "================ NHAP THU CONG MOT THIET BI ================\n";
        cin.ignore();
        cout << "Nhap Ten mat hang: ";
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
        cout << "\n=> Da nhap thanh cong thiet bi (Seri: " << serial << ")!\n";
        pauseScreen();
    }

    // 2. Nhập hàng hàng loạt bằng file (chọn từ danh sách)
   void inputItemsFromFile() {
    clearScreen();
    cout << "================ DANH SACH FILE DU LIEU CO SAN ================\n";

    vector<string> availableFiles;

    // Đọc danh sách file trong thư mục hiện tại bằng dirent.h
    DIR* dir = opendir(".");
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            string filename = entry->d_name;
            // Lọc các file có đuôi .txt hoặc .csv
            if (filename.length() >= 4) {
                string ext = filename.substr(filename.length() - 4);
                if (ext == ".txt" || ext == ".csv") {
                    availableFiles.push_back(filename);
                }
            }
        }
        closedir(dir);
    } else {
        cout << "=> Loi: Khong the mo thu muc hien tai!\n";
        pauseScreen();
        return;
    }

    if (availableFiles.empty()) {
        cout << "=> Khong tim thay file .txt hoac .csv nao trong thu muc hien tai!\n";
        pauseScreen();
        return;
    }

    // Hiển thị danh sách từng file
    for (size_t i = 0; i < availableFiles.size(); ++i) {
        cout << i + 1 << ". " << availableFiles[i] << "\n";
    }
    // Các tùy chọn nâng cao
    cout << "98. Chon CAC FILE CAN NHAP (Vi du nhap: 1 3 hoac 1,3)\n";
    cout << "99. Chon TAT CA (Nhap toan bo file tren)\n";
    cout << "0. Quay lai menu chinh\n";
    cout << "----------------------------------------------------------------\n";
    cout << "Nhan so de chon: ";

    int mainChoice;
    cin >> mainChoice;

    if (mainChoice == 0) return;

    vector<string> filesToProcess;

    if (mainChoice == 99) {
        // Nhập tất cả file
        filesToProcess = availableFiles;
    } 
    else if (mainChoice == 98) {
        // Xóa màn hình cũ trước khi cho người dùng nhập danh sách file
        clearScreen();
        cout << "================ NHAP DANH SACH FILE DU LIEU ================\n";
        
        // Hiển thị lại danh sách file rút gọn để tiện tra cứu số thứ tự
        for (size_t i = 0; i < availableFiles.size(); ++i) {
            cout << i + 1 << ". " << availableFiles[i] << "\n";
        }
        cout << "----------------------------------------------------------------\n";

        cin.ignore();
        cout << "Nhap cac so thu tu file can nhap (cach nhau bang dau cach hoac phay): ";
        string inputStr;
        getline(cin, inputStr);

        // Thay dấu phẩy thành khoảng trắng để đọc bằng stringstream
        for (char &c : inputStr) {
            if (c == ',') c = ' ';
        }

        stringstream ssInput(inputStr);
        int fileIndex;
        while (ssInput >> fileIndex) {
            if (fileIndex >= 1 && fileIndex <= static_cast<int>(availableFiles.size())) {
                filesToProcess.push_back(availableFiles[fileIndex - 1]);
            } else {
                cout << "=> Canh bao: Bo qua so thu tu khong hop le (" << fileIndex << ")\n";
            }
        }

        if (filesToProcess.empty()) {
            cout << "=> Khong co file hop le nao duoc chon!\n";
            pauseScreen();
            return;
        }
    } 
    else if (mainChoice >= 1 && mainChoice <= static_cast<int>(availableFiles.size())) {
        // Nhập 1 file đơn lẻ
        filesToProcess.push_back(availableFiles[mainChoice - 1]);
    } 
    else {
        cout << "=> Lua chon khong hop le!\n";
        pauseScreen();
        return;
    }

    clearScreen();
    int totalSuccess = 0;
    cout << "================ KET QUA NHAP DU LIEU ================\n";

    // Đọc từng file trong danh sách cần xử lý
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

    cout << "----------------------------------------------------------------\n";
    cout << "=> TONG CONG: Da nhap thanh cong " << totalSuccess << " thiet bi!\n";
    pauseScreen();
}

    // 3. Hiển thị & Sắp xếp danh sách
    void displayItems() {
        clearScreen();
        if (items.empty()) {
            cout << "\n=> Kho hang hien dang rong!\n";
            pauseScreen();
            return;
        }

        int choice;
        cout << "================ TUY CHON XUAT DANH SACH ================\n";
        cout << "1. Gia ban tu CAO den THAP\n";
        cout << "2. Gia ban tu THAP den CAO\n";
        cout << "3. Mat hang MOI nhap (Moi nhat len tren)\n";
        cout << "4. Mat hang CU (Nhap dau tien len tren)\n";
        cout << "Lua chon cua ban: ";
        cin >> choice;

        vector<PhoneItem> sortedItems = items;

        switch (choice) {
            case 1:
                sort(sortedItems.begin(), sortedItems.end(), [this](const PhoneItem& a, const PhoneItem& b) {
                    return getModelById(a.modelId).sellingPrice > getModelById(b.modelId).sellingPrice;
                });
                break;
            case 2:
                sort(sortedItems.begin(), sortedItems.end(), [this](const PhoneItem& a, const PhoneItem& b) {
                    return getModelById(a.modelId).sellingPrice < getModelById(b.modelId).sellingPrice;
                });
                break;
            case 3:
                sort(sortedItems.begin(), sortedItems.end(), [](const PhoneItem& a, const PhoneItem& b) {
                    return a.importOrder > b.importOrder;
                });
                break;
            case 4:
                sort(sortedItems.begin(), sortedItems.end(), [](const PhoneItem& a, const PhoneItem& b) {
                    return a.importOrder < b.importOrder;
                });
                break;
            default:
                cout << "=> Lua chon khong hop le!\n";
                pauseScreen();
                return;
        }

        clearScreen();
        cout << "================ DANH SACH DANH MUC KHO HANG ================\n";
        cout << "--------------------------------------------------------------------------------\n";
        cout << "| " << left << setw(22) << "Ten mat hang"
             << "| " << setw(18) << "So Seri"
             << "| " << setw(12) << "Gia nhap"
             << "| " << setw(12) << "Gia ban" << " |\n";
        cout << "--------------------------------------------------------------------------------\n";

        for (const auto& item : sortedItems) {
            PhoneModel m = getModelById(item.modelId);
            cout << "| " << left << setw(22) << m.name
                 << "| " << setw(18) << item.serialNumber
                 << "| " << setw(12) << fixed << setprecision(0) << m.importPrice
                 << "| " << setw(12) << m.sellingPrice << " |\n";
        }
        cout << "--------------------------------------------------------------------------------\n";
        pauseScreen();
    }

    // 4. CHỨC NĂNG TƯ VẤN KHÁCH HÀNG THEO TÀI CHÍNH
    void adviseCustomer() {
        clearScreen();
        cout << "================ TU VAN MUA DIEN THOAI ================\n";
        if (items.empty()) {
            cout << "=> Kho hang hien dang rong, khong the tu van!\n";
            pauseScreen();
            return;
        }

        double budget;
        cout << "Nhap so tien hien co cua khach hang (VND): ";
        cin >> budget;

        // Tìm các mặt hàng còn hàng trong kho và có giá <= ngân sách
        vector<PhoneModel> affordableModels;
        for (const auto& m : models) {
            if (m.sellingPrice <= budget && countStockByModelId(m.modelId) > 0) {
                affordableModels.push_back(m);
            }
        }

        if (affordableModels.empty()) {
            cout << "\n=> Rat tiec, khong co san pham nao trong kho phu hop voi ngan sach "
                 << fixed << setprecision(0) << budget << " VND cua khach hang!\n";
            pauseScreen();
            return;
        }

        // Sắp xếp các model phù hợp theo giá tăng dần
        sort(affordableModels.begin(), affordableModels.end(), [](const PhoneModel& a, const PhoneModel& b) {
            return a.sellingPrice < b.sellingPrice;
        });

        clearScreen();
        cout << "================ KET QUA TU VAN BAN HANG ================\n";
        cout << "Ngan sach khach hang: " << fixed << setprecision(0) << budget << " VND\n";
        cout << "--------------------------------------------------------------------------------\n";
        cout << "| " << left << setw(18) << "Tieu chi"
             << "| " << setw(24) << "Ten mat hang"
             << "| " << setw(14) << "Gia ban"
             << "| " << setw(14) << "Tiet kiem" << " |\n";
        cout << "--------------------------------------------------------------------------------\n";

        // Option 1: Tối ưu chi phí (Rẻ nhất)
        PhoneModel budgetOpt = affordableModels.front();
        cout << "| " << left << setw(18) << "1. Toi uu chi phi"
             << "| " << setw(24) << budgetOpt.name
             << "| " << setw(14) << budgetOpt.sellingPrice
             << "| " << setw(14) << (budget - budgetOpt.sellingPrice) << " |\n";

        // Option 2: Trung bình (Nếu có từ 3 sản phẩm trở lên)
        if (affordableModels.size() >= 3) {
            int midIndex = affordableModels.size() / 2;
            PhoneModel midOpt = affordableModels[midIndex];
            cout << "| " << left << setw(18) << "2. Trung binh"
                 << "| " << setw(24) << midOpt.name
                 << "| " << setw(14) << midOpt.sellingPrice
                 << "| " << setw(14) << (budget - midOpt.sellingPrice) << " |\n";
        }

        // Option 3: Tốt nhất (Cận ngân sách nhất)
        if (affordableModels.size() >= 2) {
            PhoneModel bestOpt = affordableModels.back();
            // Nếu chỉ có 2 sản phẩm thì đặt tên option 2 là Tốt nhất
            string optName = (affordableModels.size() == 2) ? "2. Tot nhat" : "3. Tot nhat";
            cout << "| " << left << setw(18) << optName
                 << "| " << setw(24) << bestOpt.name
                 << "| " << setw(14) << bestOpt.sellingPrice
                 << "| " << setw(14) << (budget - bestOpt.sellingPrice) << " |\n";
        }

        cout << "--------------------------------------------------------------------------------\n";
        pauseScreen();
    }
};

int main() {
    InventoryManager manager;
    int choice;

    do {
        clearScreen();
        cout << "================ QUAN LY KHO DIEN THOAI ================\n";
        cout << "1. Nhap hang thu cong (1 thiet bi)\n";
        cout << "2. Nhap hang HANG LOAT bang FILE (Chon bang so)\n";
        cout << "3. Xuat danh sach (Theo gia / Thoi gian nhap)\n";
        cout << "4. Tu van ban hang (Theo ngan sach khach hang)\n";
        cout << "0. Thoat\n";
        cout << "Nhan so de chon: ";
        cin >> choice;

        switch (choice) {
            case 1: manager.inputItemManual(); break;
            case 2: manager.inputItemsFromFile(); break;
            case 3: manager.displayItems(); break;
            case 4: manager.adviseCustomer(); break;
            case 0: clearScreen(); cout << "Tam biet!\n"; break;
            default: 
                cout << "Lua chon khong hop le!\n";
                pauseScreen();
        }
    } while (choice != 0);

    return 0;
}