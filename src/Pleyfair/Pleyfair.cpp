#include "Pleyfair.hpp"

using namespace pleyfair;


static const char _al[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
    "abcdefghijklmnopqrstuvwxyz"
    "абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
    " !@\"#№$;%^:&?*()-_+={}[]\\/<>.,~`"
    "0123456789";

Pleyfair::Pleyfair() {
}


bool Pleyfair::CheckKey() {
    for (int i=1; i <= Key.Length(); i++) {
        for (int j=1; j <= Key.Length(); j++) {
            if ((Key[i] == Key[j]) && (i not_eq j)) {
                return false;
            }
        }
    }
    return true;
}


void Pleyfair::CreateMatrix() {
    Form1->Memo3->Clear();
    R = Key;

    AnsiString Alphabet = Al;

    for (int i=1; i <= Key.Length(); i++) {
        for (int j=1; j <= Alphabet.Length(); j++) {
            if (Key[i] == Alphabet[j]) {
                Alphabet = Alphabet.Delete(j, 1);
            }
        }
    }

    R += Alphabet;   // создали строку с решеткой

    int num = 1;
    AnsiString Str = "";

    for (int i=0; i<10; i++) {
        Str = "";

        for (int j=0; j<16; j++) {
            Str += R[num];
            if (j != 16)
            Str += " ";
            num++;
        }
        Form1->Memo3->Lines->Add(Str);
    }

    // заполняем массив букв решетки
    num = 1;
    for (int i=0; i<10; i++) {
        for (int j=0; j<16; j++) {
            Resh[i][j] = R[num];
            num++;
        }
    }
}


void Pleyfair::ChooseEmptySymbol() {
    int k = 0;
    int m = 1000;

    for (int i=1; i<=R.Length(); i++) {
        for (int j=1; j<=MyText.Length(); j++) {
            if (R[i] == MyText[j]) {
                k++;
            }
        }

        if (k < m) {
            m = k;
            k = 0;
            Pust = R[i];
        }
    }
}


void Pleyfair::EditText() {
    ChooseEmptySymbol();

    for (int i = 1; i < MyText.Length(); i++) {
        if (MyText[i] == MyText[i+1]) {
            MyText = MyText.Insert(Pust, i + 1);
        }
    }

    int Len = MyText.Length();

    if (Len % 2 not_eq 0) {
        MyText += Pust;
    }
}
