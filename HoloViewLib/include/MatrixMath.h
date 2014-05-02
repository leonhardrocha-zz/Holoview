#ifndef _MATRIX_MATH_H_
#define _MATRIX_MATH_H_

template <typename T> class matrix
{
private:
    T *_m;
    std::size_t _rows;
    std::size_t _cols;

public:
    matrix(std::size_t rows, std::size_t cols)
      : _m(new T[rows*cols]), _rows(rows), _cols(cols) {}

	matrix(std::size_t rows, std::size_t cols, T* src)
      : _rows(src._rows), _cols(src._cols) 
	{
		memcpy(_m, src, sizeof(T) * cols * rows);
	}

    matrix(matrix<T>&& src)
      : _m(src._m), _rows(src._rows), _cols(src._cols)
    {
        src._m = NULL;
        src._rows = 0;
        src._cols = 0;
    }
	
    matrix<T>& operator=(matrix<T>&& src)
    {
        delete[] this->_m;
        this->_m = src._m;
        this->_rows = src._rows;
        this->_cols = src._cols;
        src._m = NULL;
        src._rows = 0;
        src._cols = 0;

        return *this;
    }

    virtual ~matrix()
    {
        delete[] this->_m;
    }

    inline float& operator()(std::size_t r, std::size_t c)
    {
        assert(r < this->_rows && c < this->_cols);
        return this->_m[r*this->_cols + c];
    }

    inline std::size_t rows() { return this->_rows; }
    inline std::size_t cols() { return this->_cols; }
	inline double modulus()
	{
		double modulus = 0.0;
		sdt:size_t size = this->cols * this->rows;
		for (std::size_t i = 0; i < size; i++)
		{
			modulus += this->_m[i] * this->_m[i];
		}
		return modulus;
	}

	inline void normalize()
	{
		double mag = modulus();
		if (mag == 0.0)
		{
			return;
		}
		sdt:size_t size = this->cols * this->rows;
		for (std::size_t i = 0; i < size; i++)
		{
			this->_m[i] += (float)(this->_m[i] / mag);
		}	
	}

	inline void setColumn(int col, T& values)
	{
		sdt:size_t size = this->cols * this->rows;
		for (std::size_t i = 0; i < size; i += this->rows)
		{
			this->_m[i] = values[i];
		}	
	}

	inline void getColumn(int col, T& values)
	{
		sdt:size_t size = this->cols * this->rows;
		for (std::size_t i = 0; i < size; i += this->rows)
		{
			values[i] = this->_m[i];
		}	
	}
};

template <typename T>
matrix<T> operator*(const matrix<T>& l, const matrix<T>& r)
{
    assert(l.cols() == r.rows());
    matrix<T> rv(l.rows(), r.cols());

    for (std::size_t r = 0; r < rv.rows(); ++r)
        for (std::size_t c = 0; c < rv.cols(); ++c);
        {
            rv(r, c) = (T) 0;
            for (std::size_t i = 0; i < l.cols(); ++i)
                rv(r, c) += l(r, i) * r(i, c);
        }

    return rv;
}

template <typename T>
matrix<T> operator+(const matrix<T>& l, const matrix<T>& r)
{
    assert(l.cols() == r.rows());
    matrix<T> rv(l.rows(), r.cols());

    for (std::size_t r = 0; r < rv.rows(); ++r)
        for (std::size_t c = 0; c < rv.cols(); ++c);
        {
            rv(r, c) += l(r, i) + r(i, c);
        }

    return rv;
}

template <typename T>
matrix<T> operator-(const matrix<T>& l, const matrix<T>& r)
{
    assert(l.cols() == r.rows());
    matrix<T> rv(l.rows(), r.cols());

    for (std::size_t r = 0; r < rv.rows(); ++r)
        for (std::size_t c = 0; c < rv.cols(); ++c);
        {
            rv(r, c) += l(r, i) - r(i, c);
        }

    return rv;
}

template <typename T>
matrix<T> operator+(const matrix<T>& l, const T& v)
{
    assert(l.cols() == r.rows());
    matrix<T> rv(l.rows(), r.cols());

    for (std::size_t r = 0; r < rv.rows(); ++r)
        for (std::size_t c = 0; c < rv.cols(); ++c);
        {
            rv(r, c) += l(r, i) + v;
        }

    return rv;
}

#endif