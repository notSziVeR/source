#include "StdAfx.h"
#include "PythonEmoticon.h"

PyObject* emoticonGetEmoticonCategories(PyObject* poSelf, PyObject* poArgs)
{
	const auto& categories = emoticon::get_emoticons_categories();

	PyObject* pyTuple = PyTuple_New(categories.size());

	size_t i = 0;
	for (const auto& category : categories)
	{
		PyTuple_SetItem(pyTuple, i++, PyString_FromString(category.name.c_str()));
	}

	return pyTuple;
}

PyObject* emoticonGetEmoticonsFromCategories(PyObject* poSelf, PyObject* poArgs)
{
	char* category_name = nullptr;

	if (!PyTuple_GetString(poArgs, 0, &category_name) )
	{
		return Py_BuildException();
	}

	else
	{
		const emoticon::emoticon_category_t* category_ptr = nullptr;
		for (const auto& category : emoticon::get_emoticons_categories())
		{
			if (category.name == category_name)
			{
				category_ptr = &category;
				break;
			}
		}

		if (!category_ptr)
		{
			return Py_BuildException();
		}

		else
		{
			const auto& emoticons = category_ptr->emoticons;
			PyObject* pyTuple = PyTuple_New(emoticons.size());

			size_t i = 0;
			for (const auto& emoticon : emoticons)
			{
				PyTuple_SetItem(pyTuple, i++, PyString_FromString(emoticon.name.c_str()));
			}

			return pyTuple;
		}
	}
}

PyObject* emoticonGetEmoticonInfoFromName(PyObject* poSelf, PyObject* poArgs)
{
	char* name = nullptr;

	if (!PyTuple_GetString(poArgs, 0, &name))
	{
		return Py_BuildException();
	}

	PyObject* poTuple = PyTuple_New(2);
	const auto* emoticon = emoticon::get_emoticon_info_from_name(name);

	PyTuple_SetItem(poTuple, 0, PyString_FromString(emoticon->image_path.c_str()));
	PyTuple_SetItem(poTuple, 0, PyString_FromString(emoticon->short_sym.c_str()));
	return poTuple;
}

PyObject* emoticonLoadEmoticonConfig(PyObject* poSelf, PyObject* poArgs)
{
	char* filename = nullptr;
	if (!PyTuple_GetString(poArgs, 0, &filename))
	{
		return Py_BuildException();
	}

	if (!emoticon::load_emoticons_config_file(filename))
	{
		TraceError("CANNOT LOAD EMOTICON CONFIG FILE!");
	}
	return Py_BuildNone();
}

PyObject* emoticonGetEmoticonText(PyObject* poSelf, PyObject* poArgs )
{

	char* textin = nullptr;
	if (!PyTuple_GetString(poArgs, 0, &textin))
	{
		return Py_BuildException();
	}

	std::string newtext = textin;
	emoticon::replace_emoticons_in_place(newtext);

	return Py_BuildValue("s", newtext.c_str());
}

void initemoticon()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetEmoticonCategories",		emoticonGetEmoticonCategories,			METH_VARARGS },
		{ "GetEmoticonsFromCategory",	emoticonGetEmoticonsFromCategories,		METH_VARARGS },
		{ "GetEmoticonInfoFromName",	emoticonGetEmoticonInfoFromName,		METH_VARARGS },
		{ "GetEmoticonText",			emoticonGetEmoticonText,				METH_VARARGS },
		{ "LoadEmoticonConfig",			emoticonLoadEmoticonConfig,				METH_VARARGS },

		{  NULL,						NULL,									NULL		 },
	};

	PyObject * poModule = Py_InitModule("emoticon", s_methods);
}