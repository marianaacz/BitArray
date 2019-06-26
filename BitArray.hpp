//
//  BitArray.hpp
//  bitArray_MAZ
//
//  Created by Mariana on 4/18/19.
//  Copyright © 2019 Mariana. All rights reserved.
//

// Project:  Bit Array
// Name: Mariana Acosta
// Submitted: 18/04/19
// I declare that the following source code was written by me, or provided
// by the instructor for this project. I understand that copying source
// code from any other source, providing source code to another student,
// or leaving my code on a public web site constitutes cheating.
// I acknowledge that  If I am found in violation of this policy this may result
// in a zero grade, a permanent record on file and possibly immediate failure of the class.
/* Reflection: This project was very hard for me, because I feel like it took me forever to know
 where to start. Once I understood what I needed to do, I started to test them one at a time until
 I wrote all the functions needed. It was very challenging because for the longest time I couldn't
 visualize what we were supposed to do, but I think this project helped me a lot and will help me a
 lot in the future because it has a ton of little useful things. I didn't enjoy this program but I
 know it will help me in the future.
 
 */


#ifndef BitArray_hpp
#define BitArray_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>

template<class IType = size_t>
class BitArray{
    
private:
    void assignBit(size_t pos, bool bit) {
        if (bit)
            set(pos);
        else
            reset(pos);
    }
    
    bool readBit(size_t pos) const{
        size_t block = pos / BITS_PER_WORD;
        size_t offset = pos % BITS_PER_WORD;
        
        return baVector[block] & (1ul << offset);
    }
    
    void set(size_t pos){
        size_t block = pos / BITS_PER_WORD;
        baVector[block] |= (1ul << pos);
    }
    
    void reset(size_t pos){
        size_t block = pos / BITS_PER_WORD;
        baVector[block] &= ~(1ul << pos);
    }
    
public:
    
    class bitproxy{
        BitArray& b;
        int pos;
        
    public:
        bitproxy(BitArray& bs, int p) : b(bs){
            pos = p;
        }
        bitproxy& operator=(bool bit){
            if (bit)
                b.baVector[pos /BITS_PER_WORD] |= (1ul << pos % BITS_PER_WORD);
            else
                b.baVector[pos / BITS_PER_WORD] &= ~(1ul << pos % BITS_PER_WORD);
            
            return *this;
        }
        bitproxy& operator=(bitproxy& bit){
            b.assignBit(pos, bit.b.readBit(bit.pos));
            return *this;
        }
        operator bool() const{
            return b.baVector[pos / BITS_PER_WORD] & (1ul << pos % BITS_PER_WORD);
        }
    };
    
    friend class bitproxy;
    
    bitproxy operator[](size_t pos){
        if (pos >= _size)
            throw std::logic_error("invalid index");
        
        return bitproxy(*this, pos);
    }
    
    enum {BITS_PER_WORD = CHAR_BIT * sizeof(IType)};
    std::vector<IType> baVector;
    size_t _size;
    
    explicit BitArray(size_t b = 0){
        this->_size = b;
        if (_size > 0)
            baVector.resize((b / BITS_PER_WORD)+1);
    }
    explicit BitArray(const std::string& bitString){
        _size = bitString.length();
        
        baVector.resize((_size / BITS_PER_WORD) + 1);
        
        for (size_t i = 0; i < bitString.length(); i++)
        {
            if (bitString[i] == '1')
                assignBit(i, true);
            else if (bitString[i] == '0')
                assignBit(i, false);
            else
                throw std::logic_error("String includes something other than a '1' or a '0'");
        }
    }

    
    BitArray(const BitArray& b) = default; // Copy constructor
    BitArray& operator=(const BitArray& b) = default; // Copy assignment
    explicit BitArray(BitArray&& b)noexcept : baVector(move(b.baVector)), _size(b._size){ // Move constructor
        std::cout << "move constructor" << std::endl;
    }
    BitArray& operator=(BitArray&& b)noexcept{ // Move assignment
        std::cout << "move assignment" << std::endl;
        return (*this = b);
    }
    size_t capacity()const{ // number of bits the current allocation can hold
        return (_size * sizeof(baVector));
    }
    
    //Mutators
    BitArray& operator+=(bool bit){ // Append a bit
        insert(_size, bit);
        return *this;
    }
    BitArray& operator+=(const BitArray& b){ // Append a BitArray
        insert(_size, b);
        return *this;
    }
    void erase(size_t pos, size_t nbits = 1){ // Remove "nbits" bits at a position
        std::string temp = this->to_string();
        
        temp.erase(pos, nbits);
        
        *this = BitArray<>(temp);
    }
    void insert(size_t pos, bool bit){ // Insert a bit at a position (slide "right")
        if (pos > _size)
            throw std::logic_error("Out of range");
        
        BitArray tempObj;
        std::string temp, newBit;
        temp = this->to_string();
        
        if (bit){
            newBit = "1";
            temp.insert(pos, newBit);
        }
        else{
            newBit = "0";
            temp.insert(pos, newBit);
        }
        
        tempObj = BitArray<>(temp);
        
        *this = tempObj;
    }
    void insert(size_t pos, const BitArray& bitArray){ // Insert an entire BitArray object
        if (pos > _size)
            throw std::logic_error("out of range");
        
        std::string newBits = bitArray.to_string();
        std::string temp = this->to_string();
        
        temp.insert(pos, newBits);
        
        *this = BitArray<>(temp);
    }
    void shrink_to_fit(){ // Discard unused, trailing vector cells
        
        int x = baVector.size()+1;
        while (_size+ BITS_PER_WORD < (baVector.size() * BITS_PER_WORD)){
            baVector.pop_back();
        }
        int y = baVector.size();
        std::cout << "shrinking from " << x << " to " << y << " words." << std::endl;
        
    }
    
    
    //Bitwise ops
    bool operator[](size_t pos) const {// DO I RETURN THE NUMBER? OR THE BIT?? //Might need offset
        if (pos >= _size)
            throw std::logic_error("Out of range");
        
        return readBit(pos);
    }
    void toggle(size_t pos){ // Toggles all bits
        if (pos >= _size)
            throw std::logic_error("Out of Range");
        
        size_t block = pos / BITS_PER_WORD;
        size_t offset = pos % BITS_PER_WORD;
        baVector[block] ^= (1u << offset);
    }
    
    void toggle(){
        for (size_t i = 0; i < _size; i++){
            toggle(i);
        }
    }
    
    BitArray operator~() const{
        BitArray temp = *this;
        temp.toggle();
        
        return temp;
    }
    // Shift operators...
    BitArray operator<<(unsigned int x) const{ //shift left
        std::string temp, newString;
        
        if (x >= _size)
            throw std::logic_error("shifted too far!");
        
        temp = this->to_string();
        newString = temp.substr(x, (_size - x));
        
        for (size_t i = 0; i < x; i++)
            newString += '0';
        
        return BitArray<>(newString);
        
    }
    
    BitArray operator>>(unsigned int x) const{ //Shift right
        std::string temp, newString;
        
        if (x >= _size)
            throw std::logic_error("shifted too far!");
        
        temp = this->to_string();
        
        for (size_t i = 0; i < x; i++)
            newString += '0';
        
        newString += temp.substr(0, (_size - x));
        
        return BitArray<>(newString);
    }
    BitArray& operator<<=(unsigned int x){
        BitArray temp = (*this << x);
        *this = temp;
        
        return *this;
    }
    BitArray& operator>>=(unsigned int x){
        BitArray temp = (*this >> x);
        *this = temp;
        
        return *this;
    }
    
    
    //Extraction ops
    BitArray slice(size_t pos, size_t count) const{ // Extracts a new sub-array
        std::string temp = this->to_string();
        std::string newString;
        
        newString = temp.substr(pos, count);
        return BitArray<>(newString);
    }
    
    //Comparison ops
    bool operator==(const BitArray& b) const{
        return b.to_string() == this->to_string();
    }
    bool operator!=(const BitArray& b) const{
        return b.to_string() != this->to_string();
    }
    bool operator<(const BitArray& b) const{
        return this->to_string() < b.to_string();
    }
    bool operator<=(const BitArray& b) const{
        return this->to_string() <= b.to_string();
    }
    bool operator>(const BitArray& b) const{
        return this->to_string() > b.to_string();
    }
    bool operator>=(const BitArray& b) const{
        return this->to_string() >= b.to_string();
    }
    
    
    //Counting ops
    size_t size() const{ // Number of bits in use in the vector
        return _size;
    }
    size_t count() const{ // The number of 1-bits present
        size_t counter = 0;
        for (size_t i = 0; i < _size; i++){
            if (readBit(i))
                counter++;
        }
        
        return counter;
    }
    bool any() const{ // Optimized version of count() > 0
        for (size_t i = 0; i < _size; i++){
            if (baVector[i] > 0)
                return true;
        }
        
        return false;
    }
    
    
    // Stream I/O(define thesein situ)
    friend std::ostream& operator<<(std::ostream& os, const BitArray& b){
        os << b.to_string();
        return os;
    }
    friend std::istream& operator>>(std::istream& is, BitArray& b){
        std::string temp;
        char myChar;
        while (is.get(myChar)){
            if (myChar != '1' && myChar != '0'){
                is.unget();
                break;
            }
            temp += myChar;
        }
        if (temp != "")
            b = BitArray<>(temp);
        else
            is.setstate(std::ios::failbit);
        return is;
    }
    //String Conversion
    std::string to_string() const{
        std::string temp = "";
        for (size_t i = 0; i < _size; i++){
            if (readBit(i))
                temp.append("1");
            else
                temp.append("0");

        }
        //std::cout << temp << std::endl;
        return temp;
    }
};

#endif /* BitArray_hpp */
