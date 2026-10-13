////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module demo:inputclass;

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	void KeyDown(unsigned int input)
	{
		// If a key is pressed then save that state in the key array.
		m_keys[input] = true;
	}

	void KeyUp(unsigned int input)
	{
		// If a key is released then clear that state in the key array.
		m_keys[input] = false;
	}

	auto IsKeyDown(unsigned int key) -> bool
	{
		// Return what state the key is in (pressed/not pressed).
		return m_keys[key];
	}

private:
	bool m_keys[256]{};
};
