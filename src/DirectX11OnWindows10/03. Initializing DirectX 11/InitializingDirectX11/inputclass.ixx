////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module demo:inputclass;


////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
export class InputClass
{
public:
	void Initialize()
	{
		// Initialize all the keys to being released and not pressed.
		for (int i = 0; i < 256; i++)
		{
			m_keys[i] = false;
		}
	}

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

	auto IsKeyDown(unsigned int input) const -> bool
	{
		// Return what state the key is in (pressed/not pressed).
		return m_keys[input];
	}

private:
	bool m_keys[256];
};
