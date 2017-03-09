#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <fstream>
//#include <vector>
//#include <map>
#include <array>
#include <ctime>
using namespace std;

void encBlock(unsigned int* v, unsigned int* w, unsigned int* k)
{
	register unsigned int v0 = v[0], v1 = v[1], i, sum = 0;
	register unsigned int delta = 0x9E3779B9;
	for (i = 0; i < 32; i++) {
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
	}
	w[0] = v0; w[1] = v1;
}

void encipher(const std::string& str, const std::string& key, std::string* out)
{
	unsigned int v[2], w[2], k[4], keyBuff[4];

	// Set array values to 0
	memset(v, 0, sizeof(v));
	memset(w, 0, sizeof(w));
	memset(k, 0, sizeof(k));
	memset(keyBuff, 0, sizeof(keyBuff));
	//out->clear();

	// Process the key
	int len = key.length();
	if (len > 16)
		len = 16;
	memcpy(keyBuff, key.c_str(), len);
	for (int i = 0; i < 4; ++i)
		k[i] = keyBuff[i];

	// Copy the input string to a buffer of size multiple of 4
	int strbuflen = str.length();
	if (strbuflen == 0)
		return;
	if ((strbuflen % 4) > 0)
		strbuflen += 4 - (strbuflen % 4);
	unsigned char* strbuf = new unsigned char[strbuflen];
	memset(strbuf, 0, strbuflen);
	memcpy(strbuf, str.c_str(), str.length());

	// Encode it!
	out->clear();
	v[1] = 0;
	for (int i = 0; i < strbuflen; i += 4)
	{
		v[0] = *(unsigned int*)&strbuf[i];

		encBlock(&v[0], &w[0], &k[0]);
		out->append((char*)&w[0], 4);

		v[1] = w[1];
	}
	out->append((char*)&v[1], 4);

	delete[] strbuf;
}
void decBlock(unsigned int* v, unsigned int* w, unsigned int* k)
{
	register unsigned int v0 = v[0], v1 = v[1], i, sum = 0xC6EF3720;
	register unsigned int delta = 0x9E3779B9;
	for (i = 0; i < 32; i++) {
		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
		sum -= delta;
		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
	}
	w[0] = v0; w[1] = v1;
}

void decipher(const std::string& str, const std::string& key, std::string* out)
{
	unsigned int v[2], w[2], k[4], keyBuff[4];

	// Set array values to 0
	memset(v, 0, sizeof(v));
	memset(w, 0, sizeof(w));
	memset(k, 0, sizeof(k));
	memset(keyBuff, 0, sizeof(keyBuff));

	// Count the number of passes that we need
	int numBlocks = str.length() / 4;
	int numPasses = numBlocks - 1;

	if (numPasses <= 0)
		return;

	// Process the key
	int len = key.length();
	if (len > 16)
		len = 16;
	memcpy(keyBuff, key.c_str(), len);
	for (int i = 0; i < 4; ++i)
		k[i] = keyBuff[i];

	// Create a temporary buffer to store the result
	unsigned char* buffer = new unsigned char[numPasses * 4 + 4];
	memset(buffer, 0, numPasses * 4 + 4);

	out->clear();
	const char* p = str.c_str();
	v[1] = *(unsigned int*)&p[numPasses * 4];
	for (int i = 0; i < numPasses; ++i)
	{
		v[0] = *(unsigned int*)&p[(numPasses - i - 1) * 4];
		decBlock(&v[0], &w[0], &k[0]);
		*(unsigned int*)&buffer[(numPasses - i - 1) * 4] = w[0];
		v[1] = w[1];
	}

	out->assign((char *)buffer, numPasses * 4);
	delete[] buffer;
}

int main()
{
	ifstream fileIn;
	ofstream fileOut;

	//string f1, f2, f3;
	string f1, f2;
	string *fileName = &f1; // File Name
	string *key = &f2; // Key 
	//string *data = &f3; // Data 
	string *name = new string;
	char choice;
	string str;
	string outdata;

	do {
		cout << "1: Encrypt\n2: Decrypt\n3: Exit\n";
		cin >> choice;

		switch (choice) {
		case '1': {
			cout << "Enter the encryption Key" << endl;
			cin.ignore();
			getline(cin, *key);
			cout << "Enter the string to encrypt" << endl;
			getline(cin, str);

			encipher(str, f2, &outdata);
			str = "0";
			cout << "\nCiphertext: " << outdata << endl;
			system("pause");
			cout << "Ciphertext saved in cipher.fstl\n" << endl;

			fileOut.open("cipher.fstl", ios::out | ios::binary);
			fileOut.write(outdata.c_str(), outdata.size());
			fileOut.close();
			system("pause");

			break;
		}
		case '2': {
			cout << "Enter the decryption Key" << endl;
			cin.ignore();
			getline(cin, *key);

			//Open file "cipher.fstl" and read the ciphertext into variable ciphertext
			cout << "Opening cipher.fstl to read ciphertext..." << endl;
			fileIn.open("cipher.fstl", ios::in | ios::binary);
			string ciphertext((istreambuf_iterator<char>(fileIn)), istreambuf_iterator<char>());
			fileIn.close();

			string decoded;
			decipher(ciphertext, f2, &decoded);
			ciphertext = "0";
			cout << "Decoded: " << decoded << endl;
			system("pause");
			cout << "Decoded message saved in output.txt\n" << endl;

			fileOut.open("output.txt", ios::out | ios::binary);
			fileOut.write(decoded.c_str(), decoded.size());
			fileOut.close();
			system("pause");

			break;
		}
		case '3': {
			exit(1);
		}
		default: cout << "Please choose 1, 2 or 3\n";
		}
	} while (choice != 3);

	// Key here
		/*cout << "Enter the encryption Key" << endl;
		getline(cin, *key);*/

		// Name system
			/*cout << "Enter the name of the file you want to open!" << endl;
			getline(cin,*fileName);*/

			// Input System
				/*fileIn.open(*fileName, ios::in | ios::binary);
				string str((istreambuf_iterator<char>(fileIn)), istreambuf_iterator<char>());

				fileIn.close();
				cout << "File Loaded Into RAM!" << endl;
				system("pause");*/

				/*string str;
				cout << "Enter the string to encrypt" << endl;
				getline(cin, str);*/

				// Tea System
					/*string outdata;
					TeaEncode(str, f2, &outdata);
					str = "0";
					cout << "Ciphertext: " << outdata << endl;
					system("pause");*/

					// Write Data
						//cout << "Ciphertext saved in cipher.fstl" << endl;
						///*getline(cin,*fileName);
						//system("pause");*/
						//fileOut.open("cipher.fstl", ios::out | ios::binary);
					 //   fileOut.write(outdata.c_str(), outdata.size());

					// End
	cout << "All Done!" << endl;
	system("pause");
}
