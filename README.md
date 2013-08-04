Imagify
=======

1) We take noun phrases from Title, First 2 sentences.  
How : Hit Yahoo server with YQL query to find the content of article url.

2) Retrieve the top 20 images from Yahoo! Boss. 
How : Using Yahoo! Boss API, you may need to get your own consumer key and consumer secret for OAuth.

3) Filter the images which does not have good description. 
How : We took 12Lac English sentences to train langauage model(Which is used part of machine translation system),
sentences with less probability score are filtered.

4) We score Semantic Text Similarity between description of images with Article Text. 
Top scored Image (greater than threshold) is inserted to the Article.
How : Right now, we have used UMTS STS system to find the semantic textual similarity between two sentences. It is roughly based on 
'Latent Semantic Analysis' with modification of wordnet relations. However we are trying to build weighted biparite matching 
score between two sentences based on wordnet+ resources and will be released soon :-)


INSTALL
========

TODO : There many portions need to be changed before running on your system, we run local server(php5, apache2) to process
all the above steps.

Greasemonkey script needs to be installed in your firefox browser, which will interact with local server and insert the image 
to article.


