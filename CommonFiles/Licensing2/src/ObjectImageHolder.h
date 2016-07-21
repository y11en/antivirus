#ifndef OBJECTIMAGEHOLDER_H
#define OBJECTIMAGEHOLDER_H

namespace KasperskyLicensing {
namespace Implementation {

class ObjectImageHolder : public IObjectImage
{
public:
	/**
	 * Constructs stored object.
	 */
	ObjectImageHolder(const char_t* name, const void* data, size_t size)
		: obj_name(name), obj_data(data), obj_size(size) {}
	/**
	 * Returns stored object name.
	 */
	virtual const char_t* GetObjectName() const
		{ return obj_name; }
	/**
	 * returns size of stored object image
	 */
	virtual unsigned int GetObjectSize() const
		{ return obj_size; }
	/**
	 * returns stored object image
	 */
	virtual const void* GetObjectImage() const
		{ return obj_data; }

private:
	const char_t*	obj_name;
	const void*		obj_data;
	size_t			obj_size;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // OBJECTIMAGEHOLDER_H