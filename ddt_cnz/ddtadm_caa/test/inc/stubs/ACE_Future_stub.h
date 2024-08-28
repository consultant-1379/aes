/*
 * ACE_Future_stub.h
 *
 *  Created on: May 19, 2015
 *      Author: qvincon
 */

#ifndef INC_ACE_FUTURE_STUB_H_
#define INC_ACE_FUTURE_STUB_H_


template <class T>
class ACE_Future
{
public:

	/// Constructor.
	ACE_Future() {}

	/// Destructor.
	~ACE_Future() {}

	int get (T &value) const
	{
		value = m_value;
		return 0;
	}

	void set(T& value)
	{
		m_value = value;
	}

	T m_value;

};

#endif /* INC_ACE_FUTURE_STUB_H_ */
