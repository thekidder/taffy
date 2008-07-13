//$Id$

#ifndef _SORE_MATRIX4X4_H_
#define _SORE_MATRIX4X4_H_

namespace SORE_Math
{
	const unsigned int AXIS_X = 0;
	const unsigned int AXIS_Y = 1;
	const unsigned int AXIS_Z = 2;
	
	template<class T>
			class Matrix4
	{
		public:
			Matrix4()
			{
				*this = Identity();
			}
			
			void Rotate(T rads, unsigned int axis)
			{
				switch(axis)
				{
					case AXIS_X:
						value[5 ] =  cos(rads);
						value[6 ] = -sin(rads);
						value[9 ] =  sin(rads);
						value[10] =  cos(rads);
						break;
					case AXIS_Y:
						value[0 ] =  cos(rads);
						value[2 ] =  sin(rads);
						value[8 ] = -sin(rads);
						value[10] =  cos(rads);
						break;
					case AXIS_Z:
						value[0 ] =  cos(rads);
						value[1 ] = -sin(rads);
						value[4 ] =  sin(rads);
						value[5 ] =  cos(rads);
						break;
					default:
						break;
				}
			}
			
			static Matrix4<T> Identity()
			{
				Matrix4 temp;
				for(unsigned int i=0;i<16;++i)
				{
					if(i == 0 || i == 5 || i == 10 || i == 15)
						temp.value[i] = static_cast<T>(1.0);
					else
						temp.value[i] = static_cast<T>(0.0);
				}
				return temp;
			}
		protected:
			T value[16];
	};
}

#endif
