Usage
=====

Installation
------------

To use Lumache, first install it using pip:

```console
(.venv) $ pip install lumache
```

Creating recipes
----------------

To retrieve a list of random ingredients, you can use the
`lumache.get_random_ingredients()` function:


<br>

The `kind` parameter should be either `"meat"`, `"fish"`, or `"veggies"`.

For example:

```python
>>> import lumache
>>> lumache.get_random_ingredients()
['shells', 'gorgonzola', 'parsley']
```
