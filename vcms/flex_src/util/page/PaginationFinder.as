package util.page
{
	import mx.collections.ArrayCollection;

	[Bindable]
	[RemoteClass(alias="com.broadengate.core.commons.pagination.Pagination")]
	public class PaginationFinder
	{
		
		/**
		 * 每页的记录数
		 */
		public var pageSize:int;
	
		/**
		 * 当前页中存放的记录,类型一般为List
		 */
		public var data:ArrayCollection;
	
		/**
		 * 总记录数
		 */
		public var totalCount:int;
	
		/**
		 * 总页数
		 */
		public var totalPage:int;
	
		/**
		 * 当前页号
		 */
		public var pageNo:int;
	
		/**
		 * 是否有上一页
		 */
		public var hasPrevious:Boolean;
	
		/**
		 * 是否有下一页
		 */
		public var hasNext:Boolean;
		
		public function PaginationBean()
		{
			//TODO: implement function
		}

	}
}