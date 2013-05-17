#pragma once

namespace pleyfair {

    class Pleyfair {
    public:
        Pleyfair();

        bool CheckKey();
        void CreateMatrix();
        void ChooseEmptySymbol();
        void EditText();

    protected:
        char _matrix[10][16];
    };
}
